/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#define BOOST_FILESYSTEM_VERSION 3

#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WServer>

#include "util.hpp"

namespace Wt {

namespace Wc {

void post(WServer* server, const std::string& app,
          const boost::function<void()>& func) {
    server->post(app, func);
}

boost::function<void()> bound_post(boost::function<void()> func) {
    WServer* server = downcast<WServer*>(wApp->environment().server());
    return boost::bind(post, server, wApp->sessionId(), func);
}

void updates_trigger() {
    wApp->triggerUpdate();
}

void updates_poster(WServer* server, WApplication* app) {
    server->post(app->sessionId(), updates_trigger);
}

std::string unique_filename() {
    using namespace boost::filesystem;
    const char* const model = "wt-classes-%%%%-%%%%-%%%%-%%%%";
    return unique_path(temp_directory_path() / model).string();
}

std::string config_value(const std::string& name, const std::string& def) {
    std::string value = def;
    if (wApp) {
        wApp->readConfigurationProperty(name, value);
    }
    return value;
}

WDateTime now() {
    return WDateTime::currentDateTime();
}

}

}

