/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_ABSTRACT_STORE_HPP_
#define WC_ABSTRACT_STORE_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>
#include <Wt/WJavaScript>

namespace Wt {

namespace Wc {

/** Base class for client-side storage.

\ingroup bindings
*/
class AbstractStore : public WContainerWidget {
public:
    /** Constructor.
    \param parent The parent widget.
    */
    AbstractStore(WContainerWidget* parent = 0);

    /** Clear all keys and the data from the engine */
    void clear_storage();

    /** Add or update the value for a given key */
    void set_item(const std::string& key, const std::string& value);

    /** Remove the key and its value from the engine */
    void remove_item(const std::string& key);

    /** Fetches the data by a key.
    \param key The key.
    \param def The default value returned if there is no such key.
    Since this requires a JavaScript call,
    the result can't be returned immediately.

    Instead, value() is emitted with the key and the value.

    value() is not guaranteed to be emitted, since it depends on JavaScript
    and can be interfered be client.
    */
    void get_value_of(const std::string& key, const std::string& def = "");

    /** The signal, emitted by get_value_of() through JavaScript */
    JSignal<std::string, std::string>& value() {
        return value_;
    }

protected:
    /** Clear all keys and the data from the engine (implementation) */
    virtual void clear_storage_impl() = 0;

    /** Add or update the value for a given key */
    virtual void set_item_impl(const std::string& key,
                               const std::string& value) = 0;

    /** Remove the key and its value from the engine */
    virtual void remove_item_impl(const std::string& key) = 0;

    /** Fetches the data by a key (implementation) */
    virtual void get_value_of_impl(const std::string& key,
                                   const std::string& def) = 0;

private:
    JSignal<std::string, std::string> value_;
};

}

}

#endif

