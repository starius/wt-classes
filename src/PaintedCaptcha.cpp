/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <Wt/WRandom>

#include "PaintedCaptcha.hpp"

namespace Wt {

namespace Wc {

PaintedCaptcha::PaintedCaptcha(WContainerWidget* parent):
    AbstractCaptcha(parent),
    passed_(false),
    is_compare_trimmed_(true),
    is_compare_nocase_(true),
    key_length_(6) {
    update_impl();
}

WValidator::State PaintedCaptcha::validate() {
    WValidator::State result = WValidator::InvalidEmpty;
    if (passed_ || prepare_key(user_key()) == prepare_key(true_key())) {
        result = WValidator::Valid;
        passed_ = true;
        disable();
    } else if (!user_key().empty()) {
        update();
        result = WValidator::Invalid;
    }
    return result;
}

const std::string& PaintedCaptcha::user_key() const {
    // FIXME
}

void PaintedCaptcha::set_key_length(int key_length) {
    key_length_ = key_length;
    update_impl();
}

void PaintedCaptcha::update_impl() {
    true_key_ = random_key();
    // TODO
}

std::string PaintedCaptcha::random_key() const {
    return WRandom::generateId(key_length());
}

std::string PaintedCaptcha::prepare_key(const std::string& key) const {
    using namespace boost::algorithm;
    std::string result = key;
    if (is_compare_trimmed_) {
        trim(result);
    }
    if (is_compare_nocase_) {
        to_lower(result);
    }
    return result;
}

}

}

