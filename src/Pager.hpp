/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_PAGER_HPP_
#define WC_PAGER_HPP_

#include "config.hpp"

#include <Wt/WContainerWidget>

#include "global.hpp"

namespace Wt {

namespace Wc {

/** Page navigation widget for WAbstractItemView.
Default pagination widget has several disadvantages:
 * it shows only First, Previous, Next, Last buttons
 * it requires translation of "First", "Previous", "Next", "Last".

In this widget, no translation is needed.

This widget shows buttons, providing access to many pages of view.
If total number of pages <= max_if_all_shown(), then buttons for all pages
are shown.
Otherwise the rules of shown buttons are as follows:
 * current - half_of_frame(), ..., current, ..., current + half_of_frame()
 * current, current + 1, current + base(), current + base() ** 2, ...
 * current, current - 1, current - base(), current - base() ** 2, ...
 * first = 1
 * last

For instance, if last = 1020, current = 1000, half_of_frame = 5, base = 2:
\verbatim
1 ...  488 ...  744 ...  872 ...  936 ...  968 ...  984 ...
... 992 ...  995 996 997 998 999 *1000* 1001 1002 1003 1004 1005 ...  1008 ...
... 1016 ...  1020
\endverbatim

To use this widget, inherit from WTableView
(or other descendant of WAbstractItemView)
and reimplement method <tt>WWidget* createPageNavigationBar()</tt>
to return <tt>new Pager(this)</tt>.

\ingroup util
*/
class Pager : public WContainerWidget {
public:
    /** Constructor */
    Pager(WAbstractItemView* view);

    /** Distance from current, within which no pages are skipped.
    Defaults to 3.
    */
    int half_of_frame() const {
        return half_of_frame_;
    }

    /** Set half_of_frame.
    \see half_of_frame().
    */
    void set_half_of_frame(int half_of_frame);

    /** Base of exponentiation.
    See description of the class.
    Defaults to 2.
    */
    int base() const {
        return base_;
    }

    /** Set base of exponentiation */
    void set_base(int base);

private:
    WAbstractItemView* view_;
    int base_;
    int half_of_frame_;

    void update();
};

}

}

#endif

