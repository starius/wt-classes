/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "config.hpp"

#define USE_SERVER_POST (defined(WC_HAVE_SERVER_POST) && \
    defined(WC_HAVE_ENVIRONMENT_SERVER))

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
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/replace.hpp>
#if !USE_SERVER_POST
#include <boost/thread.hpp>
#endif
#if !USE_WIOSERVICE
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#endif

#ifdef WC_USE_WT_MD5
#include <Wt/Utils>
#endif
#ifdef WC_USE_OPENSSL
#include <openssl/md5.h>
#endif

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WServer>
#ifdef WC_HAVE_WIOSERVICE
#include <Wt/WIOService>
#endif

#include "util.hpp"
#include "rand.hpp"
#include "TimeDuration.hpp"

namespace Wt {

namespace Wc {

#if USE_SERVER_POST
void post(WServer* server, const std::string& app,
          const boost::function<void()>& func) {
    server->post(app, func);
}
#else
void do_func(boost::function<void()> func, WApplication* app) {
    if (!app->isQuited()) {
        WApplication::UpdateLock lock = app->getUpdateLock();
        func();
    }
}

void thread_func(boost::function<void()> func, WApplication* app) {
    boost::thread(do_func, func, app);
}
#endif

boost::function<void()> bound_post(boost::function<void()> func) {
#if USE_SERVER_POST
    WServer* server = DOWNCAST<WServer*>(wApp->environment().server());
    return boost::bind(post, server, wApp->sessionId(), func);
#else
    return boost::bind(thread_func, func, wApp);
#endif
}

struct OneAnyFuncBinder {
    void operator()() {
        func(*arg_ptr);
    }
    OneAnyFunc func;
    boost::shared_ptr<boost::any> arg_ptr;
};

struct OneAnyFuncHolder {
    void operator()(const boost::any& arg) {
        *arg_ptr = arg;
        posted_binder();
    }
    boost::function<void()> posted_binder;
    boost::shared_ptr<boost::any> arg_ptr;
};

OneAnyFunc one_bound_post(const OneAnyFunc& func) {
    OneAnyFuncBinder binder;
    OneAnyFuncHolder holder;
    binder.func = func;
    binder.arg_ptr = boost::make_shared<boost::any>();
    holder.arg_ptr = binder.arg_ptr;
    holder.posted_binder = bound_post(binder);
    return holder;
}

void updates_trigger() {
    if (wApp->updatesEnabled()) {
        wApp->triggerUpdate();
    }
}

void updates_poster(WServer* server, WApplication* app) {
#if USE_SERVER_POST
    server->post(app->sessionId(), updates_trigger);
#else
    bound_post(updates_trigger)();
#endif
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

#ifdef WC_HAVE_MD5
std::string md5(const std::string& data) {
#ifdef WC_USE_WT_MD5
    return Wt::Utils::hexEncode(Wt::Utils::md5(data));
#elif defined(WC_USE_OPENSSL)
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
#endif
}
#endif

std::string urlencode(const std::string& url) {
    std::stringstream result;
    result.setf(std::ios::hex, std::ios::basefield);
    result.setf(std::ios::uppercase);
    result.width(2);
    result.fill('0');
    BOOST_FOREACH (char c, url) {
        if (isalnum(c) && (0 < c && c < 127)) {
            result.put(c);
        } else {
            result.put('%');
            result << short(c);
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

#define USE_WIOSERVICE (defined(WC_HAVE_WIOSERVICE) && \
        defined(WC_HAVE_ENVIRONMENT_SERVER))

#if !USE_WIOSERVICE
typedef boost::asio::deadline_timer Timer;
typedef boost::shared_ptr<Timer> TimerPtr;

struct WcIoService {
    WcIoService():
        work(io) {
        boost::thread(boost::bind(&boost::asio::io_service::run, &io));
    }

    ~WcIoService() {
        io.stop();
    }

    boost::asio::io_service io;
    boost::asio::io_service::work work;
} wc_io;

void handle_timeout(TimerPtr /* timer */,
                    const boost::function<void()>& func,
                    const boost::system::error_code& e) {
    if (!e) {
        func();
    }
}
#endif

void schedule_action(const td::TimeDuration& wait,
                     const boost::function<void()>& func) {
#if USE_WIOSERVICE
    int ms = wait.total_milliseconds();
    if (ms < 0) {
        ms = INT_MAX;
    }
    WIOService& io = wApp->environment().server()->ioService();
    io.schedule(ms, func);
#else
    TimerPtr timer = boost::make_shared<Timer>(boost::ref(wc_io.io), wait);
    timer->async_wait(boost::bind(handle_timeout, timer, func,
                                  boost::asio::placeholders::error));
#endif
}

std::string approot() {
#ifdef WC_HAVE_WAPPLICATION_APPROOT
    return WApplication::appRoot();
#else
    return config_value("approot");
#endif
}

}

}

