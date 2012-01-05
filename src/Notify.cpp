
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <Wt/WServer>
#include <Wt/WApplication>

#include "Notify.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

namespace notify {

Widget::Widget(const Event::Key& key, Server* server, const std::string& a):
    key_(key), server_(server),
    app_id_(a.empty() ? wApp->sessionId() : a) {
    server_->start_listenning(key_, this, app_id_);
}

Widget::~Widget() {
    server_->stop_listenning(key_, this, app_id_);
}

Server::Server(WServer* server):
    server_(server ? server : WServer::instance())
{ }

void Server::emit(Event* event) {
    boost::mutex::scoped_lock(mutex_);
    typename O2W::iterator it = o2w_.find(event->key());
    if (it != o2w_.end()) {
        BOOST_FOREACH (const typename A2W::value_type& a2w, it->second) {
            const std::string& app_id = a2w.first;
            const Widgets& widgets = a2w.second;
            BOOST_FOREACH (Widget* w, widgets) {
                server_->post(app_id, boost::bind(&Server::notify_widget, w));
            }
            server_->post(app_id, updates_trigger);
        }
    }
}

void Server::start_listenning(const Event::Key& key, Widget* widget,
                              const std::string& app_id) {
    boost::mutex::scoped_lock(mutex_);
    o2w_[key][app_id].push_back(widget);
}

void Server::stop_listenning(const Event::Key& key, Widget* widget,
                             const std::string& app_id) {
    boost::mutex::scoped_lock(mutex_);
    Widgets& widgets = o2w_[key][app_id];
    widgets.erase(std::find(widgets.begin(), widgets.end(), widget));
    if (widgets.empty()) {
        o2w_[key].erase(app_id);
        if (o2w_[key].empty()) {
            o2w_.erase(key);
        }
    }
}

void Server::notify_widget(Widget* widget) {
    widget->notify();
}

}

}

}

