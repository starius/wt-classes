/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/make_shared.hpp>

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WWebWidget>

#include "AdBlockDetector.hpp"
#include "rand.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

struct DefaultLists {
    DefaultLists() {
        banner_libs = boost::make_shared<AdBlockDetector::Libs>();
        b("http://js.adscale.de/getads.js", "adscale");
        b("http://view.binlayer.com/ad-00000.js", "blLayer");
        b("http://ads.adtiger.de/adscript.php", "adspirit_pid");
        b("http://pagead2.googlesyndication.com/pagead/show_ads.js",
          "google_ad_block");
        regular_libs = boost::make_shared<AdBlockDetector::Libs>();
        r("http://cdnjs.cloudflare.com/ajax/libs/underscore.js/"
          "1.3.1/underscore-min.js", "_");
    }

    void b(const std::string& url, const std::string& symbol) {
        banner_libs->push_back(std::make_pair(url, symbol));
    }

    void r(const std::string& url, const std::string& symbol) {
        regular_libs->push_back(std::make_pair(url, symbol));
    }

    AdBlockDetector::LibsPtr banner_libs;
    AdBlockDetector::LibsPtr regular_libs;
} default_lists;

AdBlockDetector::AdBlockDetector(WContainerWidget* parent, bool call_start):
    WContainerWidget(parent),
    signal_(this, "abdetector"),
    local_banner_image_(false),
    local_regular_image_(false),
    remote_banner_js_(false),
    remote_regular_js_(false),
    banner_ids_hidden_(false),
    banner_libs_(default_lists.banner_libs),
    regular_libs_(default_lists.regular_libs) {
    if (call_start) {
        start();
    }
}

bool AdBlockDetector::has_adblock(bool true_if_maybe) const {
    bool adblock = false;
    bool maybe = false;
    adblock |= local_regular_image_ && !local_banner_image_;
    maybe |= !local_banner_image_;
    if (wApp->environment().ajax()) {
        adblock |= remote_regular_js_ && !remote_banner_js_;
        maybe |= !remote_banner_js_;
        adblock |= banner_ids_hidden_;
    }
    return adblock || (maybe && true_if_maybe);
}

void AdBlockDetector::start() {
    if (wApp->environment().ajax()) {
        signal_.connect(this, &AdBlockDetector::signal_handler);
        check_remote_js(true);
        check_remote_js(false);
    }
}

void AdBlockDetector::set_js_params(std::string& url, std::string& symbol,
                                    bool banner) const {
    const Libs& libs = banner ? *banner_libs_ : *regular_libs_;
    int i = rr(libs.size());
    url = libs[i].first;
    symbol = libs[i].second;
}

void AdBlockDetector::signal_handler(std::string name) {
    if (name == "remote_regular_js") {
        remote_regular_js_ = true;
    } else if (name == "remote_banner_js") {
        remote_banner_js_ = true;
    } else if (name == "banner_ids_hidden") {
        banner_ids_hidden_ = true;
    }
}

void AdBlockDetector::check_remote_js(bool banner) {
    std::string js_url, js_symbol;
    set_js_params(js_url, js_symbol, banner);
    std::string name = "remote_";
    name += (banner ? "banner" : "regular");
    name += "_js";
    // FIXME loadScript() is undocumented Wt JavaScript function
    doJavaScript("loadScript('" + js_url + "', function() {"
                 "if (typeof " + js_symbol + " != 'undefined') {" +
                 signal_.createCall(WWebWidget::jsStringLiteral(name)) +
                 "}});");
}

}

}

