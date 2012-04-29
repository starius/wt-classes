
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <Wt/WServer>
#include <Wt/WApplication>

#include "Notify.hpp"

namespace Wt {

namespace Wc {

namespace notify {

Event::operator Event::Key() const {
    return key();
}

Widget::Widget(const Event::Key& key, Server* server, const std::string& /*a*/):
    key_(key), server_(server), app_id_(wApp) {
    server_->start_listening(this);
}

Widget::~Widget() {
    server_->stop_listening(this, app_id_);
}

Server::Server(WServer* /* server */):
    updates_enabled_(true)
{ }

void Server::emit(EventPtr event) {
    boost::mutex::scoped_lock lock(mutex_);
    O2W::iterator it = o2w_.find(event->key());
    if (it != o2w_.end()) {
        BOOST_FOREACH (const A2W::value_type& a2w, it->second) {
            WApplication* app_id = a2w.first;
            a2f_[app_id](event);
        }
    }
}

void Server::emit(Event* event) {
    emit(EventPtr(event));
}

void Server::start_listening(Widget* widget) {
    boost::mutex::scoped_lock lock(mutex_);
    WApplication* app_id = wApp;
    o2w_[widget->key()][app_id].push_back(widget);
    if (a2f_.find(app_id) == a2f_.end()) {
        a2f_[app_id] = one_bound_post(boost::bind(&Server::notify_widgets,
                                      this, _1));
    }
}

void Server::stop_listening(Widget* widget, WApplication* app_id) {
    boost::mutex::scoped_lock lock(mutex_);
    Widgets& widgets = o2w_[widget->key()][app_id];
    widgets.erase(std::find(widgets.begin(), widgets.end(), widget));
    if (widgets.empty()) {
        o2w_[widget->key()].erase(app_id);
        if (o2w_[widget->key()].empty()) {
            o2w_.erase(widget->key());
        }
        a2f_.erase(app_id);
    }
}

void Server::notify_widgets(const boost::any& event) {
    mutex_.lock();
    const EventPtr* e = boost::any_cast<EventPtr>(&event);
    Widgets widgets = o2w_[(*e)->key()][wApp];
    mutex_.unlock();
    bool updates_needed = false;
    BOOST_FOREACH (Widget* widget, widgets) {
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

