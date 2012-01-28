/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/algorithm/string/case_conv.hpp>

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WRandom>

#include "Gather.hpp"
#include "SWFStore.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

const std::string cookie_key = "userid";
const std::string swf_key = "userid";

Gather::Gather(const DataExplorer& explorer, WObject* parent):
    WObject(parent),
    explorer_(explorer), swfstore_(0), signal_(this, "gather") {
    explore_simple();
    explore_cookie();
    explore_javascript();
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
    } else if (type == JAVA) {
        return 5;
    } else {
        return 0;
    }
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
        wApp->setCookie(cookie_key, WRandom::generateId(), five_years);
    }
}

void Gather::explore_javascript() {
    signal_.connect(this, &Gather::explorer_emitter_helper);
    doJavaScript("var plugin_names = [];"
                 "for (var i = 0; i < navigator.plugins.length; i++) {"
                 "if (navigator.plugins[i].name) {"
                 "plugin_names.push(navigator.plugins[i].name);"
                 "} }"
                 "plugin_names.sort();" +
                 signal_.createCall(TO_S(PLUGINS), "plugin_names.join('|')"));
    doJavaScript("var mimes = [];"
                 "for (var i = 0; i < navigator.mimeTypes.length; i++) {"
                 "if (navigator.mimeTypes[i].suffixes) {"
                 "mimes.push(navigator.mimeTypes[i].suffixes.toLowerCase());"
                 "} }"
                 "mimes.sort();" +
                 signal_.createCall(TO_S(MIME_TYPES), "mimes.join('|')"));
    doJavaScript(signal_.createCall(TO_S(SCREEN), "'' + screen.width + ',' + "
                                    "screen.height + ',' + screen.colorDepth"));
    doJavaScript(signal_.createCall(TO_S(JAVA), "navigator.javaEnabled()"));
}

void Gather::explore_swf() {
    if (swfstore_) {
        swfstore_->value().connect(this, &Gather::swf_handler);
        swfstore_->get_value_of(swf_key);
    }
}

void Gather::explorer_emitter(DataType type, const std::string& value) {
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

void Gather::swf_handler(std::string key, WString value) {
    if (key == swf_key) {
        if (!value.empty()) {
            explorer_emitter(SWF, value.toUTF8());
        } else if (swfstore_) {
            swfstore_->set_item(swf_key, WRandom::generateId());
        }
    }
}

void Gather::doJavaScript(const std::string& javascript) {
    wApp->doJavaScript(javascript);
}

}

}

