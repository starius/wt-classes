/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WContainerWidget>

#include "ConstrainedSpinBox.hpp"
#include "util.hpp"
#include "config.hpp"

namespace Wt {

namespace Wc {

#ifdef HAVE_WSPINBOX
ConstrainedSpinBox::ConstrainedSpinBox(WContainerWidget* parent) :
    WSpinBox(parent)
{ }

int ConstrainedSpinBox::corrected_value() const {
    return constrained_value(minimum(), value(), maximum());
}
#endif

#ifdef HAVE_WDOUBLESPINBOX
ConstrainedDoubleSpinBox::ConstrainedDoubleSpinBox(WContainerWidget* parent) :
    WDoubleSpinBox(parent)
{ }

double ConstrainedDoubleSpinBox::corrected_value() const {
    return constrained_value(minimum(), value(), maximum());
}
#endif

}

}

