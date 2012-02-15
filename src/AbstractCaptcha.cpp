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
    in_progress_(false),
    is_solved_(false)
{ }

void AbstractCaptcha::check() {
    if (is_solved_) {
        solved_.emit();
    } else if (!in_progress_) {
        in_progress_ = true;
        check_impl();
    }
}

void AbstractCaptcha::update() {
    in_progress_ = false;
    is_solved_ = false;
    update_impl();
}

void AbstractCaptcha::solve() {
    in_progress_ = false;
    is_solved_ = true;
    solved_.emit();
}

}

}

