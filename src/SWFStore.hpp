/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_SWF_STORE_HPP_
#define WC_SWF_STORE_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>
#include <Wt/WJavaScript>
#include <Wt/WString>

namespace Wt {

namespace Wc {

/** A widget storing persistent data, using the Flash Player.
This class uses YUI: SWFStore.
By default, JavaScript files are downloaded from yui.yahooapis.com.

It is not a visual component, and does not generally require a visible SWF.

This widget requires JavaScript and Flash Player 9.0.115 or higher.

See YUI: SWFStore doc for more information.

All methods check whether the SWFStore is ready and postpone
actions if needed.

\attention swfstore.swf must be stored locally at the same "directory"
    as the widget does.
    But the file "storage-whitelist.xml" must be "404 Not Found"
    or contain valid xml (http://developer.yahoo.com/yui/swfstore/#security).
    If you let things slide, Wt will output some HTML for
    "storage-whitelist.xml", breaking this tool.

SWFStore example:
\include examples/swfstore.cpp.ex

\ingroup protection
\ingroup bindings
*/
class SWFStore : public WContainerWidget {
public:
    /** Constructor.
    \param parent The parent widget.
    \param load_javascript Whether needed YUI JavaScript libs should
        be loaded from yui.yahooapis.com. <br>
        Passing load_javascript = false, this is up to you to
        load all needed libs using WApplication::require().
    \param share_data Whether to share data across browsers.
    \param use_compression Whether to compress data when stored.
    */
    SWFStore(WContainerWidget* parent = 0, bool load_javascript = true,
             bool share_data = true, bool use_compression = true);

    /** Clear all keys and the data from the engine */
    void clear_storage();

    /** Add or update the value for a given key.
    \todo quotaExceededError
    */
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

private:
    JSignal<std::string, std::string> value_;

    void async_do(const std::string& js);
};

}

}

#endif

