/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "config.hpp"

#include <boost/version.hpp>
#if BOOST_VERSION >= 104400
#define BOOST_FILESYSTEM_VERSION 3
#endif

#include <sstream>
#include <climits>
#include <cstdio>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <openssl/md5.h>
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WServer>
#ifdef HAVE_WIOSERVICE
#include <Wt/WIOService>
#endif

#include "util.hpp"
#include "rand.hpp"
#include "TimeDuration.hpp"

namespace Wt {

namespace Wc {

void post(WServer* server, const std::string& app,
          const boost::function<void()>& func) {
    server->post(app, func);
}

boost::function<void()> bound_post(boost::function<void()> func) {
    WServer* server = DOWNCAST<WServer*>(wApp->environment().server());
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
    using namespace std;
#if BOOST_FILESYSTEM_VERSION == 3
    const char* const model = "wt-classes-%%%%-%%%%-%%%%-%%%%";
    return unique_path(temp_directory_path() / model).string();
#else
    string result;
    for (int attempt = 0; attempt < 10; attempt++) {
        string path = tmpnam(NULL);
        ofstream file_out(path.c_str());
        if (file_out.is_open()) {
            int secret = rr();
            file_out << secret << endl;
            file_out.close();
            if (exists(path)) {
                ifstream file_in(path.c_str());
                if (file_in.is_open()) {
                    int test;
                    file_in >> test;
                    file_in.close();
                    file_out.open(path.c_str(), ios::out | ios::trunc);
                    file_out.close();
                    if (test == secret) {
                        result = path;
                        break;
                    }
                }
            }
        }
    }
    return result;
#endif
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

std::string md5(const std::string& data) {
    const unsigned char* d;
    d = reinterpret_cast<const unsigned char*>(data.c_str());
    unsigned long n = data.size();
    unsigned char* digest = MD5(d, n, NULL);
    std::stringstream result;
    result << std::hex;
    result.width(2);
    result.fill('0');
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        result.width(2);
        result << static_cast<unsigned int>(digest[i]);
    }
    return result.str();
}

std::string urlencode(const std::string& url) {
    std::stringstream result;
    result.setf(std::ios::hex | std::ios::uppercase);
    BOOST_FOREACH (char c, url) {
        if (isalnum(c) && (0 < c && c < 127)) {
            result.put(c);
        } else {
            result.put('%');
            result << c;
        }
    }
    return result.str();
}

void set_hidden(WWidget* widget, bool hidden) {
    if (hidden) {
        widget->hide();
    } else {
        widget->show();
    }
}

std::string bool_to_string(bool value) {
    return value ? "true" : "false";
}

#ifdef HAVE_WIOSERVICE
void schedule_action(WIOService* io, const td::TimeDuration& wait,
                     const boost::function<void()>& func) {
    int ms = wait.total_milliseconds();
    if (ms < 0) {
        ms = INT_MAX;
    }
    io->schedule(ms, func);
}
#endif

std::string approot() {
#ifdef HAVE_WAPPLICATION_APPROOT
    return WApplication::appRoot();
#else
    return config_value("approot");
#endif
}

}

}

