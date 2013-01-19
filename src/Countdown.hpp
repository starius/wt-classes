/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_COUNTDOWN_HPP_
#define WC_COUNTDOWN_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>
#include <Wt/WJavaScript>
#include <Wt/WString>
#include <Wt/WDateTime>

#include "global.hpp"
#include "TimeDuration.hpp"

namespace Wt {

namespace Wc {

/** A countdown (or count-up) to a given time.
The widget is implemented as a div,
displayed inline (see setInline(true)).

For JavaScript, <a href="http://keith-wood.name/countdown.html">
jquery countdown plugin</a> is used.
Currently, only compact form of jquery countdown is used.

For HTML version, countdown is displayed by C++.
You may use WTimer to update the page periodically.

By default, the widget counts up from now.

\ingroup time
\ingroup bindings
*/
class Countdown : public WContainerWidget {
public:
    /** Constructor.
    \param parent The parent.
    \param load_javascript Whether needed jQuery, jQuery Countdown libs should
        be loaded from
    https://ajax.googleapis.com/ajax/libs/jquery/1.7.2/jquery.min.js
    and
    https://raw.github.com/starius/wt-classes/gh-pages/jquery.countdown.min.js
    . <br>
        Passing load_javascript = false, this is up to you to
        load all needed libs using WApplication::require().
    */
    Countdown(WContainerWidget* parent = 0, bool load_javascript = true);

    /** Destructor */
    ~Countdown();

    /** Set the time to count up from.
    \note Setting \c since or \c until, you invalidate another one.
    */
    void set_since(const WDateTime& since);

    /** Set the time duration, relative to the current time, to count up from.
    \note Setting \c since or \c until, you invalidate another one.
    */
    void set_since(const td::TimeDuration& since);

    /** Set the time to count down to.
    \note Setting \c since or \c until, you invalidate another one.
    */
    void set_until(const WDateTime& until);

    /** Set the time duration, relative to the current time, to count down to.
    \note Setting \c since or \c until, you invalidate another one.
    */
    void set_until(const td::TimeDuration& until);

    /** Set the format for the countdown display.
    The format string may contain following characters (in order),
    indicating the periods to display:
     - 'Y' for years,
     - 'O' for months,
     - 'W' for weeks,
     - 'D' for days,
     - 'H' for hours,
     - 'M' for minutes,
     - 'S' for seconds.

    Lowercase character mean, that the period is displayed only if non-zero.
    If one optional period is shown, all the ones after that are also shown.
    */
    void set_format(const std::string& format = "dHMS");

    /** Set the separator between the various parts of the countdown time */
    void set_time_separator(const std::string& time_separator = ":");

    /** Low-level method to change settings of jquery countdown.
    \param name Name of the setting. It is always stringified.
    \param value Value of the setting.
    \param stringify_value If the value should be stringified.
    For HTML-version, does nothing.
    */
    void change(const std::string& name, const std::string& value,
                bool stringify_value = false);

    /** Stop the countdown but don't clear it */
    void pause();

    /** Stop the countdown but don't clear it after the duration.
    Time-related change of the timeout will cancel this task.
    */
    void pause(const td::TimeDuration& duration);

    /** Stop the display but continue the countdown */
    void lap();

    /** Stop the display but continue the countdown after the duration.
    Time-related change of the timeout will cancel this task.
    */
    void lap(const td::TimeDuration& duration);

    /** Restart a paused or lap countdown */
    void resume();

    /** Restart a paused or lap countdown after the duration.
    Time-related change of the timeout will cancel this task.
    */
    void resume(const td::TimeDuration& duration);

#ifndef DOXYGEN_ONLY
private:
    struct Expired {
        Countdown* countdown;
        JSignal<>* operator->() const;
        JSignal<>& operator*() const;
        operator JSignal<>*() const;
        ~Expired();
    };
#endif

public:
    /** JavaScript signal, emitted when the countdown expires.
    This JavaScript signal collects JavaScript specified for connected slots
    or learned by stateless slot learning.

    \see "alwaysExpire" parameter of jquery.countdown

    \note This works only in JavaScript version.
        Since this relies on JavaScript, there are no guarantees
        of correctness and accuracy of this signal.
    */
#ifdef DOXYGEN_ONLY
    JSignal<>* expired();
#else
    Expired expired();
#endif

    /** Get min time duration considered significant */
    const td::TimeDuration unit() const {
        return unit_;
    }

    /** Set min time duration considered significant.
    Since time changes while the request is processed,
    countdown may have enough time to tick one time,
    causing already-paused countdowns to display wrong value.

    To address this problem, current time is memorized and considered
    constant during unit().

    Defaults to 100ms.
    */
    void set_unit(const td::TimeDuration unit) {
        unit_ = unit;
    }

private:
    class View;

    td::TimeDuration unit_;
    mutable WDateTime now_;
    View* view_;
    JSignal<>* expired_;

    WDateTime current_time() const;
    static std::string duration_for_js(const td::TimeDuration& duration);
    void do_js(const std::string& js);
    void do_js(const std::string& js, const td::TimeDuration& duration);
    void apply_js(const std::string& args);
    std::string wrap_js(const std::string& args) const;
    void update_view();
    void pause_html(const td::TimeDuration& duration);
    void lap_html(const td::TimeDuration& duration);
    void resume_html(const td::TimeDuration& duration);
    std::string pause_js() const;
    std::string lap_js() const;
    std::string resume_js() const;
    void before_time_change();
    void after_time_change();
    void clear_timeout();

};

}

}

#endif

