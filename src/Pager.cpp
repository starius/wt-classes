/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <set>
#include <algorithm>
#include <boost/bind.hpp>

#include <Wt/WAbstractItemView>
#include <Wt/WPushButton>
#include <Wt/WText>

#include "Pager.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

Pager::Pager(WAbstractItemView* view):
    view_(view), base_(2), half_of_frame_(3) {
    update();
    view_->pageChanged().connect(this, &Pager::update);
}

void Pager::set_half_of_frame(int half_of_frame) {
    half_of_frame_ = half_of_frame;
    update();
}

void Pager::set_base(int base) {
    base_ = base;
    update();
}

void Pager::update() {
    clear();
    std::set<int> pages;
    int first = 0;
    int last = view_->pageCount() - 1;
    if (last == 0) {
        return; // only one page
    }
    pages.insert(0);
    pages.insert(view_->pageCount() - 1);
    int current = view_->currentPage();
    int frame_first = current - half_of_frame();
    int frame_last = current + half_of_frame();
    for (int page = frame_first; page <= frame_last; page++) {
        if (page >= first && page <= last) {
            pages.insert(page);
        }
    }
    for (int diff_sign = -1; diff_sign <= 1; diff_sign += 2) {
        int diff = 1;
        for (int page = current;
                page >= first && page <= last;
                diff *= base(), page = current + diff_sign * diff) {
            pages.insert(page);
        }
    }
    //
    std::vector<int> pages_vector(pages.begin(), pages.end());
    std::sort(pages_vector.begin(), pages_vector.end());
    //
    for (int i = 0; i < pages_vector.size(); i++) {
        int page = pages_vector[i];
        if (i > 0 && pages_vector[i - 1] < page - 1) {
            addWidget(new WText(" ... "));
        }
        WPushButton* button = new WPushButton(TO_S(page + 1));
        addWidget(button);
        if (page == current) {
            button->disable();
        } else {
            button->clicked().connect(
                boost::bind(&WAbstractItemView::setCurrentPage,
                            view_, page));
        }
    }
}

}

}

