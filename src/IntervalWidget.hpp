/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_INTERVAL_WIDGET_HPP_
#define WC_INTERVAL_WIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WTemplate>

namespace Wt {

namespace Wc {

/** Interval input widget.
This widget can be used to input interval of any type, e.g. of time.

<h3>i18n</h3>
This template uses internationalization key wc.time.interval_template.

\ingroup time
*/
class IntervalWidget : public Wt::WTemplate {
public:
    /** Constructor.
    Two widgets passed are used as beginning and end of the interval.
    */
    IntervalWidget(Wt::WWidget* min, Wt::WWidget* max,
                   Wt::WContainerWidget* parent = 0);
};

}

}

#endif

