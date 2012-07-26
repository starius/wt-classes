/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/algorithm/string/case_conv.hpp>

#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include "Gather.hpp"
#include "SWFStore.hpp"
#include "rand.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

const std::string cookie_key = "userid";
const std::string swf_key = "userid";

Gather::Gather(const DataExplorer& explorer, WObject* parent):
    WObject(parent),
    explorer_(explorer), swfstore_(0), signal_(this, "gather"),
    honor_dnt_(false), dnt_(false) {
    const WEnvironment& env = wApp->environment();
    if (env.headerValue("DNT") == "1" ||
            env.headerValue("Dnt") == "1" ||
            env.headerValue("dnt") == "1") {
        dnt_ = true;
    }
    bound_post(boost::bind(&Gather::explore_all, this))();
}

void Gather::set_swfstore(SWFStore* swfstore) {
    swfstore_ = swfstore;
    explore_swf();
}

// FIXME random numbers?? need statictical data
int Gather::significance(DataType type) {
    if (type == COOKIE || type == SWF) {
        return 100;
    } else if (type == IP) {
        return 45;
    } else if (type == PLUGINS || type == MIME_TYPES) {
        return 30;
    } else if (type == USER_AGENT) {
        return 25;
    } else if (type == SCREEN) {
        return 20;
    } else if (type == LOCALE) {
        return 10;
    } else if (type == TIMEZONE_OFFSET) {
        return 5;
    } else if (type == JAVA) {
        return 5;
    } else {
        return 0;
    }
}

void Gather::explore_all() {
    explore_simple();
    explore_cookie();
    explore_javascript();
    explore_swf();
}

void Gather::explore_simple() {
    const WEnvironment& env = wApp->environment();
    explorer_emitter(IP, env.clientAddress());
    explorer_emitter(USER_AGENT, env.userAgent());
    explorer_emitter(LOCALE, boost::algorithm::to_lower_copy(env.locale()));
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
    doJavaScript(signal_.createCall(TO_S(JAVA), "navigator.javaEnabled()"));
}

void Gather::explore_swf() {
    if (swfstore_) {
        swfstore_->value().connect(this, &Gather::swf_handler);
        swfstore_->get_value_of(swf_key);
    }
}

void Gather::explorer_emitter(DataType type, const std::string& value) {
    if (honor_dnt() && dnt_) {
        return;
    }
    if (significance(type) && !value.empty()) {
        if (value.size() > MAX_SIZE) {
            std::string w(value);
            w.resize(MAX_SIZE);
            explorer_(type, w);
        } else {
            explorer_(type, value);
        }
    }
}

void Gather::explorer_emitter_helper(int type, std::string value) {
    explorer_emitter(static_cast<DataType>(type), value);
}

void Gather::swf_handler(std::string key, std::string value) {
    if (key == swf_key) {
        if (!value.empty()) {
            explorer_emitter(SWF, value);
        } else if (swfstore_) {
            swfstore_->set_item(swf_key, rand_string());
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

