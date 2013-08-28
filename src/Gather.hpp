/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_GATHER_HPP_
#define WC_GATHER_HPP_

#include <vector>

#include <Wt/WGlobal>
#include <Wt/WObject>
#include <Wt/WJavaScript>

#include "global.hpp"

namespace Wt {

namespace Wc {

/** \defgroup protection Tools to protect the application from misuse
These tools may be useful for fighting against bots,
virtuals or counter cheating.

The example below combines swfstore and gathering.
See Gather and SWFStore for more information.

Visit the page twice in several browsers and it should print the same
random identifier (swfstore).
\include examples/swfstore.cpp

*/

/** Utility class for gathering information about a user.
A set of tools to gather information of user's computer/browser/ip.

This class needs the fully created WApplication::instance().

\attention In some countries you should notify a user about using
    such tools and/or storing gathered information.
    Maybe, there are countries where it is illegal to use such tools.

\include examples/gather.cpp.ex
Possible output:
<pre>
[info] "Gathered: type=30, value=127.0.0.1"
[info] "Gathered: type=60, value=Mozilla/5.0 (compatible; Konqueror/2.2.2)"
[info] "Gathered: type=80, value=en-us"
[info] "Gathered: type=10, value=espxKEGpGa4aFndo"
[info] "Gathered: type=40, value=Default Plug-in|Shockwave Flash"
[info] "Gathered: type=50, value=avi,wma,wmv|mid,midi|mov|mp3|mp4|swf|wav"
[info] "Gathered: type=70, value=1280,1024,24"
[info] "Gathered: type=85, value=-240"
[info] "Gathered: type=90, value=1"
[info] "Gathered: type=20, value=M5dTk9WPiDPD4Xxx"
</pre>

\ingroup protection
*/
class Gather : public WObject {
public:
    /** Type of browser data */
    enum DataType {
        COOKIE = 10, /**< Cookie value (significant virtuals evidence) */
        SWF = 20, /**< SWF value (significant virtuals evidence) */
        LOCAL_STORAGE = 25, /**< localStorage (significant virtuals evidence) */
        IP = 30, /**< IP address (medium virtuals evidence) */
        PLUGINS = 40, /**< Plugins sorted list (medium virtuals evidence) */
        MIME_TYPES = 50, /**< Mime types sorted list (medium evidence) */
        USER_AGENT = 60, /**< User agent (weak virtuals evidence) */
        SCREEN = 70, /**< Screen resolution, depth (weak virtuals evidence) */
        LOCALE = 80, /**< Browser locale (weak virtuals evidence) */
        TIMEZONE_OFFSET = 85, /**< Timezone offset (weak virtuals evidence) */
        JAVA = 90 /**< Java enabled (weak virtuals evidence) */
    };

    /** Max allowed length of data value */
    const static unsigned int MAX_SIZE = 240;

    /** Min significant sum of \ref significance() "significances" */
    const static int MIN_SIGNIFICANT = 100;

    /** Function to be applied to each peace of information discovered */
    typedef boost::function<void(DataType, const std::string&)> DataExplorer;

    /** Constructor.
    \param explorer Function to be applied to information discovered.
    \param parent The parent.

    \attention Explorer function can be called all time this object exists.

    The function is not called if the data is empty.
    Data is not validated and provided as is
    (it may be unsafe to browse as HTML).
    The value is cropped to MAX_SIZE chars maximum.
    */
    Gather(const DataExplorer& explorer, WObject* parent = 0);

    /** Add object dealing with client-only data.
    \note Ownership of the store is not transferred.
    */
    void add_store(AbstractStore* store, DataType type);

    /** Set SWF store to use information from browser shared flash storage.
    \note Ownership of the swfstore is not transferred.
    */
    void set_swfstore(SWFStore* swfstore);

    /** Add localStorage.
    \note Ownership of the swfstore is not transferred.
    */
    void set_localstore(LocalStore* localstorage);

    /** Return if Do Not Track header is honored.
    If Do Not Track header is honored and DNT: 1,
    then DataExplorer will not be called.
    \note Currently (Wt <= 3.2.2) Header checking in Wt is case sensitive.
        Headers "Dnt" and "dnt" are also checked.

    By default, false.
    */
    bool honor_dnt() const {
        return honor_dnt_;
    }

    /** Set if Do Not Track header is honored.
    \see honor_dnt().
    */
    void set_honor_dnt(bool honor_dnt) {
        honor_dnt_ = honor_dnt;
    }

    /** Significance of virtuals evidence (in case of exact match of values).
    Return some integer from [0, MIN_SIGNIFICANT],
    where MIN_SIGNIFICANT means significant evidence,
    0 means no evidence. Sum of these integers may be used for rough result.
    If the sum is more or equal to MIN_SIGNIFICANT,
    the pair of users may be considered virtuals.
    For more accurate result, use manual check of evidence.
    */
    static int significance(DataType type);

    /** String representation of data type */
    static std::string type_to_str(DataType type);

    /** \name (Re)explore data */
    /* @{ */

    /** Call explorer for all data */
    void explore_all();

    /** Call explorer with IP, USER_AGENT and LOCALE.
    The method is called from the constructor through bound_post().

    Locale is transformed to lower case.
    */
    void explore_simple();

    /** Call explorer with COOKIE.
    The method is called from the constructor through bound_post().

    You may want to recall this method later since
    the explorer is not called if the cookie has just been created.
    */
    void explore_cookie();

    /** Call explorer with PLUGINS, MIME_TYPES, SCREEN, TIMEZONE_OFFSET, JAVA.
    The method is called from the constructor through bound_post().

    The result is got through JavaScript call,
    so the explorer is called not immediately.

    Mime types are transformed to lower case.
    */
    void explore_javascript();

    /** Call explorer with added client-side stores.
    The method is called from the constructor through bound_post().

    The result is got through JavaScript call,
    so the explorer is called not immediately.
    */
    void explore_stores();

    /** Call explorer with SWF.
    The method is called from set_swfstore().

    \deprecated Use explore_stores() instead.
    */
    void explore_swf();

    /* @} */

private:
    DataExplorer explorer_;
    struct StoreAndType {
        AbstractStore* store;
        DataType type;
    };
    std::vector<StoreAndType> stores_;
    JSignal<int, std::string> signal_; // int is DataType
    bool honor_dnt_;
    bool dnt_;

    void explorer_emitter(DataType type, const std::string& value);
    void explorer_emitter_helper(int type, std::string value);
    void store_handler(std::string key, std::string value,
                       DataType type, AbstractStore* store);
    static void doJavaScript(const std::string& javascript);
    void get_js_list(DataType type, const std::string& collection,
                     const std::string& property);
};

}

}

#endif

