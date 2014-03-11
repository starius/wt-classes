/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>

#include "SWFStore.hpp"
#include "util.hpp"
#include "config.hpp"

namespace Wt {

namespace Wc {

SWFStore::SWFStore(WContainerWidget* parent, bool load_javascript,
                   bool share_data, bool use_compression):
    AbstractStore(parent) {
    resize(0, 0);
    if (load_javascript) {
        // TODO move to https CDN
        wApp->require("http://yui.yahooapis.com/combo?"
                      "2.9.0/build/yahoo-dom-event/yahoo-dom-event.js&"
                      "2.9.0/build/element/element-min.js&"
                      "2.9.0/build/cookie/cookie-min.js&"
                      "2.9.0/build/swf/swf-min.js&"
                      "2.9.0/build/swfstore/swfstore-min.js",
                      "YAHOO.util.SWFStore");
    }
    doJavaScript("var swfstore = new YAHOO.util.SWFStore('" + id() + "', " +
                 bool_to_string(share_data) + "," +
                 bool_to_string(use_compression) + ");"
                 "$(" + jsRef() + ").data('swfstore', swfstore);"
                 "$(" + jsRef() + ").data('ready', false);"
                 "swfstore.addListener('contentReady', function() {"
                 "$(" + jsRef() + ").data('ready', true);"
                 "});");
}

void SWFStore::clear_storage_impl() {
    async_do("$(" + jsRef() + ").data('swfstore').clear();");
}

void SWFStore::set_item_impl(const std::string& key, const std::string& value) {
    async_do("$(" + jsRef() + ").data('swfstore')"
             ".setItem('" + key + "', '" + value + "');");
}

void SWFStore::remove_item_impl(const std::string& key) {
    async_do("$(" + jsRef() + ").data('swfstore')"
             ".removeItem('" + key + "');");
}

void SWFStore::get_value_of_impl(const std::string& key,
                                 const std::string& def) {
    async_do("var value = $(" + jsRef() + ").data('swfstore')"
             ".getValueOf('" + key + "') || '" + def + "';" +
             value().createCall("'" + key + "'", "value"));
}

void SWFStore::async_do(const std::string& js) {
    doJavaScript("if ($(" + jsRef() + ").data('ready')) {" +
                 js +
                 "} else {"
                 "$(" + jsRef() + ").data('swfstore')"
                 ".addListener('contentReady', function() {" +
                 js +
                 "});"
                 "}");
}

}

}

