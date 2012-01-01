
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <Wt/WServer>
#include <Wt/WApplication>

#include "util.hpp"

namespace Wt {

namespace Wc {

namespace notify {

template <typename E>
Widget<E>::Widget(const E& e, Server<E>* server, const std::string& a):
    event_(e), server_(server),
    app_id_(a.empty() ? wApp->sessionId() : a) {
    server_->start_listenning(event_, this, app_id_);
}

template <typename E>
Widget<E>::~Widget() {
    server_->stop_listenning(event_, this, app_id_);
}

template <typename E>
Server<E>::Server(WServer* server):
    server_(server ? server : WServer::instance())
{ }

template <typename E>
void Server<E>::emit(const E& event) {
    boost::mutex::scoped_lock(mutex_);
    typename O2W::iterator it = o2w_.find(event);
    if (it != o2w_.end()) {
        BOOST_FOREACH (const typename A2W::value_type& a2w, it->second) {
            const std::string& app_id = a2w.first;
            const Widgets& widgets = a2w.second;
            BOOST_FOREACH (Widget<E>* w, widgets) {
                server_->post(app_id, boost::bind(&Server::notify_widget, w));
            }
            server_->post(app_id, updates_trigger);
        }
    }
}

template <typename E>
void Server<E>::start_listenning(const E& event, Widget<E>* widget,
                                 const std::string& app_id) {
    boost::mutex::scoped_lock(mutex_);
    o2w_[event][app_id].push_back(widget);
}

template <typename E>
void Server<E>::stop_listenning(const E& event, Widget<E>* widget,
                                const std::string& app_id) {
    boost::mutex::scoped_lock(mutex_);
    Widgets& widgets = o2w_[event][app_id];
    widgets.erase(std::find(widgets.begin(), widgets.end(), widget));
    if (widgets.empty()) {
        o2w_[event].erase(app_id);
        if (o2w_[event].empty()) {
            o2w_.erase(event);
        }
    }
}

template <typename E>
void Server<E>::notify_widget(Widget<E>* widget) {
    widget->notify();
}

}

}

}

