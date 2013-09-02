/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>
#include <boost/assert.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include "CachedContents.hpp"

namespace Wt {

namespace Wc {

CachedContents::CachedContents(WContainerWidget* parent):
    WContainerWidget(parent), cache_size_(10), current_widget_(0)
{ }

CachedContents::~CachedContents() {
    clear();
}

void CachedContents::open_url(const std::string& url) {
    std::string fixed_url = (!url.empty() && *url.rbegin() == '/') ?
                            url : url + "/";
    if (is_ignored(fixed_url)) {
        open_url_impl(fixed_url);
    } else {
        Url2Widget::iterator it = url_to_widget_.find(fixed_url);
        if (it == url_to_widget_.end()) {
            open_url_impl(fixed_url);
            url_to_widget_[fixed_url] = current_widget_;
            resize_cache();
            visited_urls_.push_back(fixed_url);
        } else {
            WWidget* widget = it->second;
            set_contents_raw(widget);
            visited_urls_.remove(fixed_url);
            visited_urls_.push_back(fixed_url);
        }
    }
}

void CachedContents::set_contents_raw(WWidget* w) {
    if (current_widget_) {
        if (wApp->environment().ajax()) {
            current_widget_->hide();
        } else {
            // HTML version, client side cache is useless
            removeWidget(current_widget_);
        }
    }
    if (wApp->environment().ajax()) {
        if (!w->parent()) {
            addWidget(w);
        } else {
            w->show();
        }
    } else {
        // HTML version, client side cache is useless
        addWidget(w);
    }
    current_widget_ = w;
}

void CachedContents::ignore_url(const std::string& url) {
    std::string fixed_url = (!url.empty() && *url.rbegin() == '/') ?
                            url : url + "/";
    ignored_urls_.insert(fixed_url);
}

void CachedContents::ignore_prefix(const std::string& prefix) {
    ignored_prefixes_.insert(prefix);
}

void CachedContents::clear() {
    BOOST_FOREACH (Url2Widget::value_type& u2w, url_to_widget_) {
        WWidget* widget = u2w.second;
        delete widget;
    }
    url_to_widget_.clear();
    visited_urls_.clear();
    current_widget_ = 0;
}

void CachedContents::resize_cache() {
    if (visited_urls_.size() > cache_size()) {
        const std::string& url = visited_urls_.front();
        Url2Widget::iterator it = url_to_widget_.find(url);
        BOOST_ASSERT(it != url_to_widget_.end());
        WWidget* widget = it->second;
        delete widget;
        url_to_widget_.erase(it);
        visited_urls_.pop_front();
    }
}

bool CachedContents::is_ignored(const std::string& url) {
    if (ignored_urls_.find(url) != ignored_urls_.end()) {
        return true;
    }
    if (!ignored_prefixes_.empty()) {
        StringsSet::const_iterator it = ignored_prefixes_.upper_bound(url);
        if (it != ignored_prefixes_.begin()) {
            it--;
            const std::string& prefix = *it;
            if (boost::starts_with(url, prefix)) {
                return true;
            }
        }
    }
    return false;
}

}

}

