
#include <algorithm>
#include <utility>
#include "boost-xtime.hpp"
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
    start_listening(key);
}

Widget::Widget(Server* server):
    server_(server), app_id_(wApp)
{ }

void Widget::start_listening(const Event::Key& key) {
    Server::WidgetAndKeyList changes;
    changes.push_back(std::make_pair(this, key));
    server_->start_listening(changes);
}

void Widget::start_listening(const Event::KeyList& keylist) {
    Server::WidgetAndKeyList changes;
    BOOST_FOREACH (const Event::Key& key, keylist) {
        changes.push_back(std::make_pair(this, key));
    }
    server_->start_listening(changes);
}

void Widget::stop_listening(const Event::KeyList& keylist) {
    Server::WidgetAndKeyList changes;
    BOOST_FOREACH (const Event::Key& key, keylist) {
        changes.push_back(std::make_pair(this, key));
    }
    server_->stop_listening(changes);
}

void Widget::stop_listening(const Event::Key& key) {
    Server::WidgetAndKeyList changes;
    changes.push_back(std::make_pair(this, key));
    server_->stop_listening(changes);
}

void Widget::stop_listening() {
    stop_listening(keylist_);
}

Widget::~Widget() {
    stop_listening();
}

void Widget::notify(EventPtr event) {
    if (wApp) {
        wApp->log("warn") << "Base class notify::Widget was notified.";
    }
}

const Event::Key Widget::key() const {
    return keylist_.empty() ? "" : keylist_[0];
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

Server::PosterPtr Server::get_poster_ptr(WApplication* app_id) {
    PosterWeakPtr& poster_weak_ptr = a2p_[app_id];
    PosterPtr poster_ptr;
    if (poster_weak_ptr.expired()) {
        OneAnyFunc notify = boost::bind(&Server::notify_widgets, this, _1);
        OneAnyFunc poster = one_bound_post(notify, merge_allowed_);
        poster_ptr = boost::make_shared<OneAnyFunc>(poster);
        poster_weak_ptr = poster_ptr;
        return poster_ptr;
    } else {
        return poster_weak_ptr.lock();
    }
}

void Server::start_listening(const WidgetAndKeyList& changes) {
    boost::mutex::scoped_lock lock(mutex_);
    WApplication* app_id = wApp;
    PosterPtr poster_ptr = get_poster_ptr(app_id);
    BOOST_FOREACH (const WidgetAndKey& widget_and_key, changes) {
        Widget* widget = widget_and_key.first;
        const Event::Key& key = widget_and_key.second;
        A2W& a2w = o2w_[key];
        if (a2w.find(app_id) == a2w.end()) {
            a2w[app_id] = std::make_pair(poster_ptr, Widgets());
        }
        Widgets& widgets = a2w[app_id].second;
        widgets.push_back(widget);
        widget->keylist_.push_back(key);
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

void Server::remove_key(Widget* widget, const Event::Key& key) {
    // remove pair from internal map
    WApplication* app_id = widget->app_id_;
    Widgets& widgets = o2w_[key][app_id].second;
    Widgets::iterator it = std::find(widgets.begin(), widgets.end(), widget);
    if (it != widgets.end()) {
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
    // remove key from widget
    Event::KeyList& keys = widget->keylist_;
    Event::KeyList::iterator kit = std::find(keys.begin(), keys.end(), key);
    if (kit != keys.end()) {
        // remove it: move back to it and pop back
        *kit = keys.back();
        keys.pop_back();
    }
}

void Server::stop_listening(const WidgetAndKeyList& changes) {
    boost::mutex::scoped_lock lock(mutex_);
    BOOST_FOREACH (const WidgetAndKey& widget_and_key, changes) {
        Widget* widget = widget_and_key.first;
        const Event::Key& key = widget_and_key.second;
        remove_key(widget, key);
        widgets_set().erase(widget);
    }
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

