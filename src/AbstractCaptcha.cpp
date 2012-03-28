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
    fault_(0),
    in_progress_(false),
    is_solved_(false)
{ }

AbstractCaptcha::~AbstractCaptcha() {
    if (fault_) {
        delete fault_;
        fault_ = 0;
    }
}

void AbstractCaptcha::check() {
    if (is_solved_) {
        solved_.emit();
    } else if (!in_progress_) {
        WString error = precheck_.empty() ? WString::Empty : precheck_();
        if (!error.empty()) {
            mistake(error);
        } else {
            in_progress_ = true;
            check_impl();
        }
    }
}

void AbstractCaptcha::update() {
    in_progress_ = false;
    is_solved_ = false;
    update_impl();
}

AbstractCaptcha::FaultSignal& AbstractCaptcha::fault() {
    if (!fault_) {
        fault_ = new FaultSignal(this);
    }
    return *fault_;
}

void AbstractCaptcha::set_buttons(bool)
{ }

void AbstractCaptcha::set_input(WFormWidget* /* input */)
{ }

void AbstractCaptcha::solve() {
    in_progress_ = false;
    is_solved_ = true;
    solved_.emit();
}

void AbstractCaptcha::mistake(const WString& message) {
    update();
    if (fault_) {
        fault_->emit(message);
    }
}

}

}

