/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>

#include "LocalStore.hpp"

namespace Wt {

namespace Wc {

LocalStore::LocalStore(WContainerWidget* parent):
    AbstractStore(parent) {
    resize(0, 0);
    doJavaScript("try {"
                 "window.localStorage.setItem('testkey', 'testvalue');"
                 "if (window.localStorage.getItem('testkey') == 'testvalue') {"
                 "$(" + jsRef() + ").data('localStorage_supported', true);"
                 "}"
                 "storage.removeItem('testkey');"
                 "} catch(e) { }");
}

void LocalStore::clear_storage_impl() {
    try_do("window.localStorage.clear();");
}

void LocalStore::set_item_impl(const std::string& key,
                               const std::string& value) {
    try_do("window.localStorage.setItem('" + key + "', '" + value + "');");
}

void LocalStore::remove_item_impl(const std::string& key) {
    try_do("window.localStorage.removeItem('" + key + "');");
}

void LocalStore::get_value_of_impl(const std::string& key,
                                   const std::string& def) {
    try_do("var value = window.localStorage"
           ".getItem('" + key + "') || '" + def + "';" +
           value().createCall("'" + key + "'", "value"));
}

void LocalStore::try_do(const std::string& js) {
    doJavaScript("if ($(" + jsRef() + ").data('localStorage_supported')) {" +
                 js +
                 "}");
}

}

}

