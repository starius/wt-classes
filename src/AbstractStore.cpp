/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "AbstractStore.hpp"

namespace Wt {

namespace Wc {

AbstractStore::AbstractStore(WContainerWidget* parent):
    WContainerWidget(parent),
    value_(this, "value")
{ }

void AbstractStore::clear_storage() {
    clear_storage_impl();
}

void AbstractStore::set_item(const std::string& key, const std::string& value) {
    set_item_impl(key, value);
}

void AbstractStore::remove_item(const std::string& key) {
    remove_item_impl(key);
}

void AbstractStore::get_value_of(const std::string& key,
                                 const std::string& def) {
    get_value_of_impl(key, def);
}

}

}

