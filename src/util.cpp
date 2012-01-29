/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WServer>

namespace Wt {

namespace Wc {

void post(const boost::function<void()>& func, WApplication* app) {
    if (!app) {
        app = wApp;
    }
    WServer* server = app->environment().server();
    server->post(app->sessionId(), func);
}

boost::function<void()> bound_post(boost::function<void()> func) {
    return boost::bind(post, func, wApp);
}

void updates_trigger() {
    wApp->triggerUpdate();
}

void updates_poster(WServer* server, WApplication* app) {
    server->post(app->sessionId(), updates_trigger);
}

}

}

