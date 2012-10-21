/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_SWF_STORE_HPP_
#define WC_SWF_STORE_HPP_

#include <Wt/WGlobal>

#include "AbstractStore.hpp"

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
class SWFStore : public AbstractStore {
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

protected:
    void clear_storage_impl();

    void set_item_impl(const std::string& key, const std::string& value);

    void remove_item_impl(const std::string& key);

    void get_value_of_impl(const std::string& key, const std::string& def);

private:
    void async_do(const std::string& js);
};

}

}

#endif

