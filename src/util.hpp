/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_UTIL_HPP_
#define WC_UTIL_HPP_

#include <boost/cast.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/any.hpp>

#include <Wt/WGlobal>
#include <Wt/WDateTime>

#include "global.hpp"

namespace Wt {

class WIOService; // FIXME http://redmine.emweb.be/issues/1189
class WServer; // not declared n WGlobal of Wt 3.1.2

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

\deprecated Use DOWNCAST
\ingroup util
*/
#define downcast boost::polymorphic_downcast

/** Downcasting operator.

\ingroup util
*/
#define DOWNCAST boost::polymorphic_downcast

/** Ugly macro converting any type to std::string using boost::lexical_cast.

\ingroup util
*/
#define TO_S(x) boost::lexical_cast<std::string>(x)

/** Return the same function, but being called afterwards.
All needed arguments are bound and new function
object is returned.

If WServer::post() is available, it is used, else thread-based implementation
is used, which is not so safe, as WServer::post() is
(if the application was deleted, it may cause segfault).

\ingroup util
*/
boost::function<void()> bound_post(boost::function<void()> func);

/** Function of one argument.

\ingroup util
*/
typedef boost::function<void(const boost::any&)> OneAnyFunc;

/** Return the same function, but being called afterwards.
This function is like bound_post(), but it allows to bind one argument.

\ingroup util
*/
OneAnyFunc one_bound_post(const OneAnyFunc& func);

/** Call triggerUpdate() in current WApplication.

\ingroup util
*/
void updates_trigger();

/** Post triggerUpdate() to the application.
If server.post() is available, it is used, else bound_post() is used.

\ingroup util
*/
void updates_poster(WServer* server, WApplication* app);

/** Return unique temp file name.
Boost.Filesystem's unique_path is used, if present.

Otherwise this function call tmpnam() from C++ Standard library,
checking the result to be writable.
If this fails 10 times, empty string is returned.

\ingroup util
*/
std::string unique_filename();

/** Read a configuration property.
The convenience method for readConfigurationProperty().
On error, \p def value is returned.

\attention wApp should be defined, else the default would be returned.

\ingroup util
*/
std::string config_value(const std::string& name, const std::string& def = "");

/** Current datetime.

\ingroup util
*/
WDateTime now();

/** Compute the MD5 message digest of the data (hex).
OpenSSL is used.

\ingroup util
*/
std::string md5(const std::string& data);

/** URL-encodes string.

\ingroup util
*/
std::string urlencode(const std::string& url);

/** Hides or shows the widget.
This is a workaround for Wt 3.1.10 setHidden changes and forgotten default.

\ingroup util
*/
void set_hidden(WWidget* widget, bool hidden);

/** Return stringified bool ("true" or "false").

\ingroup util
*/
std::string bool_to_string(bool value);

#ifdef WC_HAVE_WIOSERVICE
/** Utility method used to schedule a function.
\attention The function would be executed in "raw" thread of io service.
    Consider using of bound_post() wrapper for \p func.
\note If the number of total milliseconds of the duration exceeds the
    size of \c int, the duration is decreased to fit into this size (INT_MAX).
    If sizeof(int) is 4, max duration is about 24.8 days.
\see WIOService::schedule()
\ingroup util
*/
void schedule_action(WIOService* io, const td::TimeDuration& wait,
                     const boost::function<void()>& func);
#endif

/** Return WApplication::appRoot() if found or config_value("approot").

\ingroup util
*/
std::string approot();

/** Return value constrained by [minimum, maximum].

\ingroup util
*/
template <typename T>
T constrained_value(T minimum, T value, T maximum) {
    if (value < minimum) {
        value = minimum;
    } else if (value > maximum) {
        value = maximum;
    }
    return value;
}

}

}

#endif

