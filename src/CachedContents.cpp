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
    WContainerWidget(parent), cache_size_(10), current_widget_(0),
    cache_title_(true)
{ }

CachedContents::~CachedContents() {
    clear_cache();
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
            WidgetAndTitle& widget_and_title = url_to_widget_[fixed_url];
            widget_and_title.first = current_widget_;
            if (cache_title_) {
                widget_and_title.second = wApp->title();
            }
            visited_urls_.push_back(fixed_url);
            resize_cache();
        } else {
            WidgetAndTitle& widget_and_title = it->second;
            WWidget* widget = widget_and_title.first;
            set_contents_raw(widget);
            if (cache_title_) {
                const WString& title = widget_and_title.second;
                wApp->setTitle(title);
            }
            visited_urls_.remove(fixed_url); // O(cache_size)
            visited_urls_.push_back(fixed_url);
        }
    }
}

void CachedContents::set_contents_raw(WWidget* w) {
    if (current_widget_) {
        bool current_is_cached = false;
        BOOST_FOREACH (const Url2Widget::value_type& u2w, url_to_widget_) {
            // O(cache_size)
            const WidgetAndTitle& w_a_t = u2w.second;
            WWidget* widget = w_a_t.first;
            if (widget == current_widget_) {
                current_is_cached = true;
                break;
            }
        }
        if (!current_is_cached) {
            // it is ignored or set by external code
            delete current_widget_;
            current_widget_ = 0;
        } else if (wApp->environment().ajax()) {
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

void CachedContents::set_cache_size(int cache_size) {
    cache_size_ = cache_size;
    resize_cache();
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
    clear_cache();
    WContainerWidget::clear();
}

void CachedContents::clear_cache() {
    BOOST_FOREACH (Url2Widget::value_type& u2w, url_to_widget_) {
        WidgetAndTitle& widget_and_title = u2w.second;
        WWidget* widget = widget_and_title.first;
        delete widget;
    }
    url_to_widget_.clear();
    visited_urls_.clear();
    current_widget_ = 0;
}

void CachedContents::remove_from_cache(const std::string& url) {
    url_to_widget_.erase(url);
}

void CachedContents::resize_cache() {
    int desired_size = cache_size();
    if (desired_size < 0) {
        desired_size = 0;
    }
    while (visited_urls_.size() > desired_size) {
        const std::string& url = visited_urls_.front();
        Url2Widget::iterator it = url_to_widget_.find(url);
        BOOST_ASSERT(it != url_to_widget_.end());
        WidgetAndTitle& widget_and_title = it->second;
        WWidget* widget = widget_and_title.first;
        if (widget != current_widget_) {
            delete widget;
        }
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

