/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_GLOBAL_LOCALIZED_STRINGS_HPP_
#define WC_GLOBAL_LOCALIZED_STRINGS_HPP_

#include <map>
#include <vector>
#include <boost/thread/mutex.hpp>

#include <Wt/WLocalizedStrings>
#include <Wt/WMessageResourceBundle>

namespace Wt {

namespace Wc {

/** Bound to server localized strings.

Usage:
\code
// global (to use Wt::Wc::approot(), WServer must exist)
GlobalLocalizedStrings global_storage;
global_storage.use(Wt::Wc::approot() + "locales");
global_storage.use(Wt::Wc::approot() + "another1");
global_storage.add_lang("ru");
global_storage.add_lang("uk");
// in session
wApp->setLocalizedStrings(global_storage->create_localized_strings());
\endcode

\warning Global storage should not be modified

\todo Refresh data, if files were changed.

\ingroup util
*/
class GlobalLocalizedStrings {
public:
    /** Constructor */
    GlobalLocalizedStrings();

    /** Destructor */
    virtual ~GlobalLocalizedStrings();

    /** Create new instance of localized strings, using this global storage */
    WLocalizedStrings* create_localized_strings();

    /** Add a message resource files to be used.
    Similar to WMessageResourceBundle::use().
    */
    void use(const std::string& path);

    /** Add a language code to be used.
    \note Default language (empty string is already added).
    */
    void add_lang(const std::string& lang);

private:
    typedef std::map<std::string, WMessageResourceBundle> Lang2Bundle;
    Lang2Bundle lang_to_bundle_;
    std::vector<std::string> paths_;
    int ref_count_;
    boost::mutex ref_count_mutex_;

    friend class GlobalLocalizedStringsPtr;
};

}

}

#endif

