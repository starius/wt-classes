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
class WDialog; // not declared n WGlobal of Wt 3.1.2
class WTextEdit; // not declared n WGlobal of Wt 3.1.2

namespace Wc {

/** \defgroup util Utility code
Useful classes, functions and macros.
*/

/** Return whether an object is an instance of a class or of a subclass thereof.
\note If \p object == 0, return 0.

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

/** Return the same function, but being called afterwards in this application.
When returned function is called, it looks as if original function were called
inside current application (wApp for the moment bound_post was called).

If wApp == 0 for the moment bound_post was called,
the target function is called outside of Wt event loop
(returned function calls schedule_action(td::TD_NULL, func)).

If WServer::post() is available, it is used, else thread-based implementation
is used, which is not so safe, as WServer::post() is:
if the application is being deleting, it may cause segfault (almost impossible).
Completely safe function post without WServer::post() seems impossible,
but this function really does work.

\ingroup util
*/
boost::function<void()> bound_post(boost::function<void()> func);

/** Function of one argument.

\ingroup util
*/
typedef boost::function<void(const boost::any&)> OneAnyFunc;

/** Return the same function, but being called afterwards.
\param func The function, called afterwards as if it is called in this app.
    When you call returned functor with boost::any, this will result
    in calling \p func with this boost::any.
\param allow_merge Whether sequential calls of returned functor
    are allowed to be merged and executed through single post.
    This is optimization of number of function posts (call of function,
    returned by bound_post()) and pushed of server initiated updates.

This function is like bound_post(), but it allows to bind one argument.

\ingroup util
*/
OneAnyFunc one_bound_post(const OneAnyFunc& func, bool allow_merge = true);

/** Call triggerUpdate() in current WApplication, if updates are enabled.

If !wApp or application is quited, does nothing.

\note This is only possible after a call to wApp->enableUpdates()

\ingroup util
*/
void updates_trigger();

/** Post updates_poster() to the application.
If server.post() is available, it is used, else bound_post() is used.

\note This is only possible after a call to wApp->enableUpdates()

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

#ifdef WC_HAVE_MD5
/** Compute the MD5 message digest of the data (hex).
Wt's built-in function or OpenSSL is used.

\ingroup util
*/
std::string md5(const std::string& data);
#endif

/** URL-encodes string.

\ingroup util
*/
std::string urlencode(const std::string& url);

/** URL-decodes string.

\ingroup util
*/
std::string urldecode(const std::string& text);

/** Hides or shows the widget.
This is a workaround for Wt 3.1.10 setHidden changes and forgotten default.

\ingroup util
*/
void set_hidden(WWidget* widget, bool hidden);

/** Return stringified bool ("true" or "false").

\ingroup util
*/
std::string bool_to_string(bool value);

/** Utility method used to schedule a function.
\attention The function would be executed in "raw" thread of io service.
    Consider using of bound_post() wrapper for \p func.
\note (For WIOService only)
    If the number of total milliseconds of the duration exceeds the
    size of \c int, the duration is decreased to fit into this size (INT_MAX).
    If sizeof(int) is 4, max duration is about 24.8 days.

This function uses WIOService::schedule(), if available,
else boost::asio::io_service with WApplication::UpdateLock is used.
\ingroup util
*/
void schedule_action(const td::TimeDuration& wait,
                     const boost::function<void()>& func);

/** Return WApplication::appRoot() if found or config_value("approot").

\ingroup util
*/
std::string approot();

/** Return value constrained by [minimum, maximum].

\ingroup util
*/
template <typename Tmin, typename Tvalue, typename Tmax>
Tvalue constrained_value(Tmin minimum, Tvalue value, Tmax maximum) {
    if (value < minimum) {
        value = minimum;
    } else if (value > maximum) {
        value = maximum;
    }
    return value;
}

/** Return value of widget if it is possible.
If form_widget->valueText() is available, it is used,
otherwise self implementation is used.

If text value can not be calculated, returns "".

\ingroup util
*/
WString value_text(const WFormWidget* form_widget);

/** Add close button (x) to the dialog.
Add close button to the titlebar, if it is possible,
otherwise to the body.

\ingroup util
*/
void set_closable(WDialog* dialog);

/** Automatically delete this dialog, when finished.

\ingroup util
*/
void delete_closed(WDialog* dialog);

/** Fix WTextEdit position.
This function was written, since WTextEdit doesn't tell its position properly
to neighbours. It looks as if it's size is 0.

\warning \p text_edit should be inserted into WContainerWidget,
    without use of layouts.
    This funtion must be called in the same javascript, which is responsible
    for the creation of WTextEdit.

If it is needed, this function replaces text_edit in it's parent with
a WContainerWidget and inserts text_edit into it. Then the WContainerWidget
is resized to size of WTextEdit + 5px vertically.
If the size of WTextEdit was not set, it is set to 525x130 pp.

\ingroup util
*/
void fix_text_edit(WTextEdit* text_edit);

/** Excape UTF-8 chars (>=128) with \uXXXX sequences for Json parser.
This function does preprocessing of JSON, containing UTF-8 chars.
(needed for Wt <= 3.2.2).

\ingroup util
*/
std::string json_escape_utf8(const std::string& utf8);

/** Scroll view to bottom (JavaScript + HTML versions).

\ingroup util
*/
void scroll_to_last(WTableView* view);

/** Return interger or 'bad' if this string can not be interpreted as integer.

\ingroup util
*/
int str2int(const std::string& str, int bad = -1);

/** Convert simple HTML anchors to Wt-like anchors.
This function start JS setInterval with period \c interval_ms.
Each time all simple anchors (created not by Wt things like WAnchor
or LinkRole) will be converted to Wt-like links.
Click on Wt-like links will not cause full page reload
and will use Wt's history functions.

If external_blank, then all external anchors will get target=_blank.

If skip_re is not empty, then it is used as regular expression for anchors
that will not be changed. For example, this can be used to treat .html files.

This function is safe to be called multiple times.

If !wApp, then does nothing.

\ingroup util
*/
void fix_plain_anchors(bool external_blank = false, int interval_ms = 400,
                       const std::string& skip_re = "/\\.htm|\\.php/i");

}

}

#endif

