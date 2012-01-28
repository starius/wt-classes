/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_GATHER_HPP_
#define WC_GATHER_HPP_

#include <Wt/WGlobal>
#include <Wt/WObject>
#include <Wt/WJavaScript>

#include "global.hpp"

namespace Wt {

namespace Wc {

/** \defgroup gather Gathering tools
A set of tools to gather information of user's computer/browser/ip.

This may ne usefull for fighting against virtuals or
prevention of counter cheating.

\attention In some countries you should notify a user about using
    such tools and/or storing gathered information.
    Maybe, there are countries where it is illegal to use such tools.
*/

/** Utility class for gathering information about a user.
This class needs the fully created WApplication::instance().

\ingroup gather
*/
class Gather : public WObject {
public:
    /** Type of browser data */
    enum DataType {
        COOKIE = 10, /**< Cookie value (significant virtuals evidence) */
        SWF = 20, /**< SWF value (significant virtuals evidence) */
        IP = 30, /**< IP address (medium virtuals evidence) */
        PLUGINS = 40, /**< Plugins sorted list (medium virtuals evidence) */
        MIME_TYPES = 50, /**< Mime types sorted list (medium evidence) */
        USER_AGENT = 60, /**< User agent (weak virtuals evidence) */
        SCREEN = 70, /**< Screen resolution, depth (weak virtuals evidence) */
        LOCALE = 80, /**< Browser locale (weak virtuals evidence) */
        JAVA = 90 /**< Java enabled (weak virtuals evidence) */
    };

    /** Max allowed length of data value */
    const static unsigned int MAX_SIZE = 240;

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

    /** Set SWF store to use information from browser shared flash storage.
    \note Ownership of the swfstore is not transferred.
    */
    void set_swfstore(SWFStore* swfstore);

    /** Significance of virtuals evidence (in case of exact match of values).
    Return some integer from [0, 100], where 100 means significant evidence,
    0 means no evidence. Sum of these integers may be used for rough result.
    For more accurate result, use manual check of evidence.
    */
    static int significance(DataType type);

    /** \name (Re)explore data */
    /* @{ */

    /** Call explorer with IP, USER_AGENT and LOCALE.
    The method is called from the constructor.

    Locale is transformed to lower case.
    */
    void explore_simple();

    /** Call explorer with COOKIE.
    The method is called from the constructor.

    You may want to recall this method later since
    the explorer is not called if the cookie has just been created.
    */
    void explore_cookie();

    /** Call explorer with PLUGINS, MIME_TYPES, SCREEN and JAVA.
    The method is called from the constructor.

    The result is got through JavaScript call,
    so the explorer is called not immediately.

    Mime types are transformed to lower case.
    */
    void explore_javascript();

    /** Call explorer with SWF.
    The method is called from set_swfstore().

    You may want to recall this method later since
    the explorer is not called if the flash cookie has just been created.

    The result is got through JavaScript call,
    so the explorer is called not immediately.
    */
    void explore_swf();

    /* @} */

private:
    DataExplorer explorer_;
    SWFStore* swfstore_;
    JSignal<int, std::string> signal_; // int is DataType

    void explorer_emitter(DataType type, const std::string& value);
    void explorer_emitter_helper(int type, std::string value);
    void swf_handler(std::string key, WString value);
    static void doJavaScript(const std::string& javascript);
    void get_js_list(DataType type, const std::string& collection,
                     const std::string& property);
};

}

}

#endif

