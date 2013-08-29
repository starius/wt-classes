/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <algorithm>
#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include <Wt/WApplication>

#include "GlobalLocalizedStrings.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

class GlobalLocalizedStringsPtr : public WLocalizedStrings {
public:
    GlobalLocalizedStringsPtr(GlobalLocalizedStrings* data):
        data_(data), bundle_(0) {
        boost::mutex::scoped_lock lock(data_->ref_count_mutex_);
        data_->ref_count_ += 1;
        update_bundle_ptr();
    }

    ~GlobalLocalizedStringsPtr() {
        boost::mutex::scoped_lock lock(data_->ref_count_mutex_);
        data_->ref_count_ -= 1;
    }

    bool resolveKey(const std::string& key, std::string& result) {
        if (bundle_) {
            return bundle_->resolveKey(key, result);
        } else {
            return false;
        }
    }

    bool resolvePluralKey(const std::string& key, std::string& result,
                          ::uint64_t amount) {
        if (bundle_) {
            return bundle_->resolvePluralKey(key, result, amount);
        } else {
            return false;
        }
    }

    void refresh() {
        update_bundle_ptr();
    }

private:
    GlobalLocalizedStrings* data_;
    WMessageResourceBundle* bundle_;

    void update_bundle_ptr() {
        std::string lang = get_locale(wApp);
        lang.resize(2); // en-US => en
        if (lang == "en") {
            lang = "";
        }
        typedef GlobalLocalizedStrings::Lang2Bundle::iterator It;
        It it = data_->lang_to_bundle_.find(lang);
        if (it != data_->lang_to_bundle_.end()) {
            bundle_ = &(it->second);
        } else {
            bundle_ = 0;
        }
    }
};

GlobalLocalizedStrings::GlobalLocalizedStrings() {
    add_lang("");
}

GlobalLocalizedStrings::~GlobalLocalizedStrings() {
    BOOST_ASSERT(ref_count_ == 0);
}

WLocalizedStrings* GlobalLocalizedStrings::create_localized_strings() {
    return new GlobalLocalizedStringsPtr(this);
}

static void add_path_to_bundle(WMessageResourceBundle& bundle,
                               const std::string& path,
                               const std::string& lang) {
    std::string full_path = path + (lang.empty() ? "" : "_") + lang;
    bundle.use(full_path);
}

void GlobalLocalizedStrings::use(const std::string& path) {
    if (std::find(paths_.begin(), paths_.end(), path) == paths_.end()) {
        paths_.push_back(path);
        BOOST_FOREACH (Lang2Bundle::value_type& lang_and_bundle,
                      lang_to_bundle_) {
            const std::string& lang = lang_and_bundle.first;
            WMessageResourceBundle& bundle = lang_and_bundle.second;
            add_path_to_bundle(bundle, path, lang);
        }
    }
}

void GlobalLocalizedStrings::add_lang(const std::string& lang) {
    if (lang_to_bundle_.find(lang) == lang_to_bundle_.end()) {
        WMessageResourceBundle& bundle = lang_to_bundle_[lang];
        BOOST_FOREACH (const std::string& path, paths_) {
            add_path_to_bundle(bundle, path, lang);
        }
    }
}

}

}

