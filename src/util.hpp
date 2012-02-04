/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WT_UTIL_HPP_
#define WT_UTIL_HPP_

#include <boost/cast.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>

#include <Wt/WGlobal>

namespace Wt {

namespace Wc {

/** \defgroup util Utility code
Useful classes, functions and macros.
*/

/** Return whether an object is an instance of a class or of a subclass thereof.
This function and it's documentation was copied from python.

\ingroup util
*/
template <class T, class S>
bool isinstance(const S* object) {
    try {
        return dynamic_cast<const T*>(object) != 0;
    } catch (...) {
        return false;
    }
}

/** Downcasting operator.

\ingroup util
*/
#define downcast boost::polymorphic_downcast

/** Ugly macro converting any type to std::string using boost::lexical_cast.

\ingroup util
*/
#define TO_S(x) boost::lexical_cast<std::string>(x)

/** Post the function to the application.
If app == 0, this means current application (wApp).

\ingroup util
*/
void post(const boost::function<void()>& func, WApplication* app = 0);

/** Return the same function, but being called through post().
post() is bound with \p func and wApp and resultion function
object is returned.

\ingroup util
*/
boost::function<void()> bound_post(boost::function<void()> func);

/** Call triggerUpdate() in current WApplication.

\ingroup util
*/
void updates_trigger();

/** Post triggerUpdate() to the application.

\ingroup util
*/
void updates_poster(WServer* server, WApplication* app);

/** Return unique temp file name.
Boost.Filesystem's unique_path is used

\ingroup util
*/
std::string unique_filename();

/** Stringify a string.
Steps:
 - escape backslashes,
 - escape double quotes,
 - surround with double quotes.

\ingroup util
*/
std::string stringify(const std::string& value);

/** Read a configuration property.
The convenience method for readConfigurationProperty().
On error, \p def value is returned.

\attention wApp should be defined, else the default would be returned.

\ingroup util
*/
std::string config_value(const std::string& name, const std::string& def = "");

}

}

#endif

