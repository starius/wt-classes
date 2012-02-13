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
    passed_(false) {
    update();
}

WValidator::State AbstractCaptcha::validate() {
    WValidator::State result = WValidator::InvalidEmpty;
    if (passed_ || user_key() == true_key()) {
        result = WValidator::Valid;
        passed_ = true;
        disable();
    } else if (!user_key().empty()) {
        update();
        result = WValidator::Invalid;
    }
    return result;
}

void AbstractCaptcha::update() {
    true_key_ = random_key();
    update_impl();
}

}

}

