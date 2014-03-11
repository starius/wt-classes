/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include "Gather.hpp"
#include "LocalStore.hpp"
#include "SWFStore.hpp"
#include "WebRTC_IP.js"
#include "rand.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

const std::string cookie_key = "userid";
const std::string store_key = "userid";

Gather::Gather(const DataExplorer& explorer, WObject* parent):
    WObject(parent),
    explorer_(explorer), signal_(this, "gather"),
    honor_dnt_(false), dnt_(false) {
    const WEnvironment& env = wApp->environment();
    if (env.headerValue("DNT") == "1" ||
            env.headerValue("Dnt") == "1" ||
            env.headerValue("dnt") == "1") {
        dnt_ = true;
    }
    bound_post(boost::bind(&Gather::explore_all, this))();
}

void Gather::add_store(AbstractStore* store, DataType type) {
    store->value().connect(boost::bind(&Gather::store_handler, this,
                                       _1, _2, type, store));
    StoreAndType sat;
    sat.store = store;
    sat.type = type;
    stores_.push_back(sat);
}

void Gather::set_swfstore(SWFStore* swfstore) {
    add_store(swfstore, SWF);
}

void Gather::set_localstore(LocalStore* localstorage) {
    add_store(localstorage, LOCAL_STORAGE);
}

// FIXME random numbers?? need statictical data
int Gather::significance(DataType type) {
    if (type == COOKIE || type == SWF || type == LOCAL_STORAGE ||
            type == ETAG || type == LAST_MODIFIED) {
        return 10000;
    } else if (type == IP) {
        return 45;
    } else if (type == WEBRTC_IP || type == WEBRTC_LAN) {
        return 50;
    } else if (type == PLUGINS || type == MIME_TYPES) {
        return 30;
    } else if (type == USER_AGENT) {
        return 25;
    } else if (type == HTTP_ACCEPT) {
        return 25;
    } else if (type == SCREEN) {
        return 20;
    } else if (type == LOCALE) {
        return 10;
    } else if (type == TIMEZONE_OFFSET) {
        return 5;
    } else if (type == TIME_ERROR) {
        return 5;
    } else if (type == PING) {
        return 5;
    } else if (type == JAVA) {
        return 5;
    } else {
        return 0;
    }
}

std::string Gather::type_to_str(DataType type) {
    if (type == COOKIE) {
        return "cookie";
    } else if (type == SWF) {
        return "flash_cookie";
    } else if (type == LOCAL_STORAGE) {
        return "local_storage";
    } else if (type == ETAG) {
        return "etag";
    } else if (type == LAST_MODIFIED) {
        return "last_modified";
    } else if (type == IP) {
        return "ip";
    } else if (type == WEBRTC_IP) {
        return "webrtc_ip";
    } else if (type == WEBRTC_LAN) {
        return "webrtc_lan";
    } else if (type == PLUGINS) {
        return "plugins";
    } else if (type == MIME_TYPES) {
        return "mime_types";
    } else if (type == USER_AGENT) {
        return "user_agent";
    } else if (type == HTTP_ACCEPT) {
        return "http_accept";
    } else if (type == SCREEN) {
        return "screen";
    } else if (type == LOCALE) {
        return "locale";
    } else if (type == TIMEZONE_OFFSET) {
        return "timezone";
    } else if (type == TIME_ERROR) {
        return "timeerror";
    } else if (type == PING) {
        return "ping";
    } else if (type == JAVA) {
        return "java";
    } else {
        return "unknown";
    }
}

void Gather::explore_all() {
    explore_simple();
    explore_cookie();
    explore_javascript();
    explore_stores();
}

void Gather::explore_simple() {
    const WEnvironment& env = wApp->environment();
    explorer_emitter(IP, env.clientAddress());
    explorer_emitter(USER_AGENT, env.userAgent());
    explorer_emitter(HTTP_ACCEPT, env.accept());
    explorer_emitter(LOCALE, boost::algorithm::to_lower_copy(get_locale()));
}

void Gather::explore_cookie() {
    const WEnvironment& env = wApp->environment();
    try {
        const std::string cookie_value = env.getCookie(cookie_key);
        explorer_emitter(COOKIE, cookie_value);
    } catch (...) {
        int five_years = 3600 * 24 * 365 * 5;
        wApp->setCookie(cookie_key, rand_string(), five_years);
    }
}

void Gather::explore_javascript() {
    signal_.connect(this, &Gather::explorer_emitter_helper);
    get_js_list(PLUGINS, "navigator.plugins", "name");
    get_js_list(MIME_TYPES, "navigator.mimeTypes", "suffixes.toLowerCase()");
    doJavaScript(signal_.createCall(TO_S(SCREEN), "'' + screen.width + ',' + "
                                    "screen.height + ',' + screen.colorDepth"));
    doJavaScript(signal_.createCall(TO_S(TIMEZONE_OFFSET),
                                    "''+(new Date()).getTimezoneOffset()"));
    doJavaScript(signal_.createCall(TO_S(TIME_ERROR),
                                    "''+Date.now() % (60 * 60 * 1000)"));
    doJavaScript(signal_.createCall(TO_S(JAVA), "navigator.javaEnabled()"));
    std::string call_ip = signal_.createCall(TO_S(WEBRTC_IP), "localIp");
    std::string call_lan = signal_.createCall(TO_S(WEBRTC_LAN), "ip");
    std::string call_ping = signal_.createCall(TO_S(PING), "ping");
    using namespace boost::algorithm;
    std::string js = replace_first_copy(WebRTC_IP_JS, "__webrtc_ip__", call_ip);
    js = replace_first_copy(js, "__webrtc_lan__", call_lan);
    js = replace_first_copy(js, "__ping__", call_ping);
    doJavaScript(js);
}

void Gather::explore_stores() {
    BOOST_FOREACH (const StoreAndType& sat, stores_) {
        sat.store->get_value_of(store_key);
    }
}

void Gather::explore_swf() {
    explore_stores();
}

void Gather::explorer_emitter(DataType type, const std::string& value) {
    if (honor_dnt() && dnt_) {
        return;
    }
    if (significance(type) && !value.empty()) {
        std::string w(value);
        if (w.size() > MAX_SIZE) {
            w.resize(MAX_SIZE);
        }
        if (type == TIME_ERROR) {
            try {
                int client_ms = boost::lexical_cast<int>(w);
                int server_ms = now().toPosixTime()
                                .time_of_day().total_milliseconds();
                int d = (client_ms - server_ms) % (60 * 60 * 1000);
                if (d > 30 * 60 * 1000) {
                    // 30 mins
                    d -= 60 * 60 * 1000;
                }
                if (d < -30 * 60 * 1000) {
                    d += 60 * 60 * 1000;
                }
                // from -30min to +30min
                w = TO_S(d);
            } catch (...) {
                return;
            }
        }
        explorer_(type, w);
    }
}

void Gather::explorer_emitter_helper(int type, std::string value) {
    explorer_emitter(static_cast<DataType>(type), value);
}

void Gather::store_handler(std::string key, std::string value,
                           DataType type, AbstractStore* store) {
    if (key == store_key) {
        if (!value.empty()) {
            explorer_emitter(type, value);
        } else {
            store->set_item(store_key, rand_string());
        }
    }
}

void Gather::doJavaScript(const std::string& javascript) {
    wApp->doJavaScript(javascript);
}

void Gather::get_js_list(DataType type, const std::string& collection,
                         const std::string& property) {
    doJavaScript("var arr = [];"
                 "for (var i = 0; i < " + collection + ".length; i++) {"
                 "var v = " + collection + "[i]." + property + ";"
                 "if (v && $.inArray(v, arr) == -1) {"
                 "arr.push(" + collection + "[i]." + property + ");"
                 "} }"
                 "arr.sort();" +
                 signal_.createCall(TO_S(type), "arr.join('|')"));
}

}

}

