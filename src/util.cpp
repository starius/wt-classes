/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>
#include <Wt/WServer>

namespace Wt {

namespace Wc {

void updates_trigger() {
    wApp->triggerUpdate();
}

void updates_poster(WServer* server, WApplication* app) {
    server->post(app->sessionId(), updates_trigger);
}

}

}

