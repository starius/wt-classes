/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_TIME_DURATION_WIDGET_HPP_
#define WC_TIME_DURATION_WIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>

#include "TimeDuration.hpp"

namespace Wt {

namespace Wc {

class TimeDurationWidgetImpl;

/** Input widget for time duration.

\ingroup time
*/
class TimeDurationWidget : public WCompositeWidget {
public:
    /** Time duration (convience typedef) */
    typedef td::TimeDuration TimeDuration;

    /** Constructor.
    \param min       The minimum value
    \param value     The default value
    \param max       The maximum value
    \param parent    The parent widget
    */
    TimeDurationWidget(const TimeDuration& min, const TimeDuration& value,
                       const TimeDuration& max, WContainerWidget* parent = 0);

    /** Return the value */
    TimeDuration value() const;

    /** Return the corrected value.
    Same as value(), but it is guaranteed to lie in [minimum(), maximum()].
    */
    TimeDuration corrected_value() const;

    /** Return the minimum value */
    TimeDuration minimum() const;

    /** Return the maximum value */
    TimeDuration maximum() const;

    /** Return the time duration used as a unit.
    This can be: second, minute, hour, etc.
    */
    TimeDuration unit() const;

    /** Get internal form widget.
    This may be used for WLabel (e.g., in TableForm).
    */
    WFormWidget* form_widget();

private:
    TimeDurationWidgetImpl* impl_;
};

}

}

#endif

