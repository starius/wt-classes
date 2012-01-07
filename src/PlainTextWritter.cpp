/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "PlainTextWritter.hpp"

namespace Wt {

namespace Wc {

PlainTextWritter::PlainTextWritter(std::ostream& out, const short cpl):
    out_(out), cpl_(cpl), current_cpl_(0)
{ }

PlainTextWritter::~PlainTextWritter() {
    out_ << std::endl;
}

void PlainTextWritter::write_word(const std::string& word) {
    if (current_cpl_ + word.length() > cpl_) {
        out_ << std::endl;
        current_cpl_ = 0;
    } else if (current_cpl_) {
        out_ << ' ';
        current_cpl_ += 1;
    }
    out_ << word;
    current_cpl_ += word.length();
}

}

}

