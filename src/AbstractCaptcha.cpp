/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include "AbstractCaptcha.hpp"

namespace Wt {

namespace Wc {

AbstractCaptcha::AbstractCaptcha(WContainerWidget* parent):
    WCompositeWidget(parent),
    passed_(false),
    is_compare_trimmed_(true),
    is_compare_nocase_(true) {
    update();
}

WValidator::State AbstractCaptcha::validate() {
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

void AbstractCaptcha::update() {
    true_key_ = random_key();
    update_impl();
}

std::string AbstractCaptcha::prepare_key(const std::string& key) const {
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

