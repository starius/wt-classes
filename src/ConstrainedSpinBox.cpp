/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WContainerWidget>

#include "ConstrainedSpinBox.hpp"

namespace Wt {

namespace Wc {

template <typename T>
T spin_box_corrected_value(T minimum, T value, T maximum) {
    if (value < minimum) {
        value = minimum;
    } else if (value > maximum) {
        value = maximum;
    }
    return value;
}

ConstrainedSpinBox::ConstrainedSpinBox(WContainerWidget* parent) :
    WSpinBox(parent)
{ }

int ConstrainedSpinBox::corrected_value() const {
    return spin_box_corrected_value(minimum(), value(), maximum());
}

ConstrainedDoubleSpinBox::ConstrainedDoubleSpinBox(WContainerWidget* parent) :
    WDoubleSpinBox(parent)
{ }

double ConstrainedDoubleSpinBox::corrected_value() const {
    return spin_box_corrected_value(minimum(), value(), maximum());
}

}

}

