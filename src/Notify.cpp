
#include <algorithm>
#include <utility>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/tss.hpp>

#include <Wt/WServer>
#include <Wt/WApplication>

#include "Notify.hpp"

namespace Wt {

namespace Wc {

namespace notify {

Event::~Event()
{ }

Event::operator Event::Key() const {
    return key();
}

Widget::Widget(const Event::Key& key, Server* server, const std::string& /*a*/):
    server_(server), app_id_(wApp) {
    keylist_.push_back(key);
    server_->start_listening(this);
}

Widget::Widget(Server* server):
    server_(server), app_id_(wApp) {
}

void Widget::start_listening(const Event::KeyList& keylist) {
    keylist_ = keylist;
    server_->start_listening(this);
}

Widget::~Widget() {
    server_->stop_listening(this, app_id_);
}

void Widget::notify(EventPtr event) {
    if (wApp) {
        wApp->log("warn") << "Base class notify::Widget was notified.";
    }
}

Server::Server(WServer* /* server */):
    updates_enabled_(true),
    direct_to_this_(false),
    merge_allowed_(true)
{ }

void Server::emit(EventPtr event) const {
    mutex_.lock();
    bool notify_in_this_app = false;
    // find any Applications interested in this event
    O2W::const_iterator it = o2w_.find(event->key());
    if (it != o2w_.end()) {
        BOOST_FOREACH (const A2W::value_type& a2w, it->second) {
            WApplication* app = a2w.first;
            if (!direct_to_this_ || app != wApp || app == 0) {
                const PosterAndWidgets& poster_and_widgets = a2w.second;
                const OneAnyFunc& poster = *(poster_and_widgets.first);
                poster(event);
            } else {
                notify_in_this_app = true;
            }
        }
    }
    mutex_.unlock();
    if (notify_in_this_app) {
        notify_widgets(event);
    }
}

void Server::emit(Event* event) const {
    emit(EventPtr(event));
}

class DummyEvent : public Event {
public:
    DummyEvent(const std::string& key):
        key_(key)
    { }

    std::string key() const {
        return key_;
    }

private:
    std::string key_;
};

void Server::emit(const std::string& key) const {
    emit(boost::make_shared<DummyEvent>(key));
}

void Server::start_listening(Widget* widget) {
    boost::mutex::scoped_lock lock(mutex_);
    WApplication* app_id = wApp;
    BOOST_FOREACH(const Event::Key key, widget->keylist() ) {
        A2W& a2w = o2w_[key];
        if (a2w.find(app_id) == a2w.end()) {
            PosterPtr poster_ptr;
            PosterWeakPtr& poster_weak_ptr = a2p_[app_id];
            if (poster_weak_ptr.expired()) {
                OneAnyFunc notify = boost::bind(&Server::notify_widgets, this, _1);
                OneAnyFunc poster = one_bound_post(notify, merge_allowed_);
                poster_ptr = boost::make_shared<OneAnyFunc>(poster);
                poster_weak_ptr = poster_ptr;
            } else {
                poster_ptr = poster_weak_ptr.lock();
            }
            a2w[app_id] = std::make_pair(poster_ptr, Widgets());
        }
        Widgets& widgets = a2w[app_id].second;
        widgets.push_back(widget);
    }
}

typedef std::set<Widget*> WidgetsSet;
typedef boost::thread_specific_ptr<WidgetsSet> WidgetsSetPtr;
WidgetsSetPtr widgets_set_ptr_;

static WidgetsSet& widgets_set() {
    if (widgets_set_ptr_.get() == 0) {
        widgets_set_ptr_.reset(new WidgetsSet());
    }
    return *widgets_set_ptr_;
}

void Server::stop_listening(Widget* widget, WApplication* app_id) {
    boost::mutex::scoped_lock lock(mutex_);
    BOOST_FOREACH (const Event::Key key, widget->keylist()) {
        Widgets& widgets = o2w_[key][app_id].second;
        Widgets::iterator it = std::find(widgets.begin(),
                widgets.end(), widget);
        *it = widgets.back();
        widgets.pop_back();
        if (widgets.empty()) {
            o2w_[key].erase(app_id);
            if (a2p_[app_id].expired()) {
                a2p_.erase(app_id);
            }
            if (o2w_[key].empty()) {
                o2w_.erase(key);
            }
        }
    }
    widgets_set().erase(widget);
}

void Server::notify_widgets(const boost::any& event) const {
    WidgetsSet& widgets_s = widgets_set();
    widgets_s.clear();
    mutex_.lock();
    const EventPtr* e = boost::any_cast<EventPtr>(&event);
    O2W::const_iterator o2w_it = o2w_.find((*e)->key());
    if (o2w_it != o2w_.end()) {
        const A2W& a2w = o2w_it->second;
        A2W::const_iterator a2w_it = a2w.find(wApp);
        if (a2w_it != a2w.end()) {
            const PosterAndWidgets& paw = a2w_it->second;
            const Widgets& widgets_v = paw.second;
            WidgetsSet widgets_s_temp(widgets_v.begin(), widgets_v.end());
            widgets_s.swap(widgets_s_temp);
        }
    }
    mutex_.unlock();
    bool updates_needed = false;
    while (!widgets_s.empty()) {
        WidgetsSet::iterator it = widgets_s.begin();
        Widget* widget = *it;
        widgets_s.erase(it);
        updates_needed |= widget->updates_needed(*e);
        widget->notify(*e);
    }
    if (updates_needed && updates_enabled_) {
        updates_trigger();
    }
}

}

}

}

