/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "AbstractCaptcha.hpp"

namespace Wt {

namespace Wc {

AbstractCaptcha::AbstractCaptcha(WContainerWidget* parent):
    WCompositeWidget(parent),
    is_solved_(false)
{ }

void AbstractCaptcha::check() {
    if (is_solved_) {
        solved_.emit();
    } else {
        check_impl();
    }
}

void AbstractCaptcha::update() {
    is_solved_ = false;
    update_impl();
}

void AbstractCaptcha::solve() {
    is_solved_ = true;
    solved_.emit();
}

}

}

