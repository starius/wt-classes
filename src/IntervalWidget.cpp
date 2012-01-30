/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "IntervalWidget.hpp"

namespace Wt {

namespace Wc {

IntervalWidget:: IntervalWidget(WWidget* min, WWidget* max,
                                WContainerWidget* parent):
    WTemplate(tr("wc.time.interval_template"), parent) {
    bindWidget("min", min);
    bindWidget("max", max);
}

}

}

