/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_LOCAL_STORE_HPP_
#define WC_LOCAL_STORE_HPP_

#include <Wt/WGlobal>

#include "AbstractStore.hpp"

namespace Wt {

namespace Wc {

/** A widget storing persistent data, using JavaScript localStorage.

\ingroup bindings
*/
class LocalStore : public AbstractStore {
public:
    /** Constructor */
    LocalStore(WContainerWidget* parent = 0);

protected:
    void clear_storage_impl();

    void set_item_impl(const std::string& key, const std::string& value);

    void remove_item_impl(const std::string& key);

    void get_value_of_impl(const std::string& key, const std::string& def);

private:
    void try_do(const std::string& js);
};

}

}

#endif

