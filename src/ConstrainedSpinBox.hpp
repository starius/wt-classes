/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_CONSTRAINED_SPIN_BOX_HPP_
#define WC_CONSTRAINED_SPIN_BOX_HPP_

#include <Wt/WGlobal>
#include <Wt/WSpinBox>
#include <Wt/WDoubleSpinBox>

namespace Wt {

namespace Wc {

/** Spin box with constrained value */
class ConstrainedSpinBox : public Wt::WSpinBox {
public:
    /** Constructor */
    ConstrainedSpinBox(Wt::WContainerWidget* parent = 0);

    /** Return the corrected value.
    Same as value(), but it is guaranteed to lie in [minimum(), maximum()].
    */
    int corrected_value() const;
};

/** Double spin box with constrained value */
class ConstrainedDoubleSpinBox : public Wt::WDoubleSpinBox {
public:
    /** Constructor */
    ConstrainedDoubleSpinBox(Wt::WContainerWidget* parent = 0);

    /** Return the corrected value.
    Same as value(), but it is guaranteed to lie in [minimum(), maximum()].
    */
    double corrected_value() const;
};

}

}

#endif

