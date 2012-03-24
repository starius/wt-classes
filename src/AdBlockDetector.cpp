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
#include <Wt/WResource>
#include <Wt/WImage>
#include <Wt/WRandom>

#include "AdBlockDetector.hpp"
#include "rand.hpp"
#include "util.hpp"

namespace Wt {
namespace Wc {
class ReporterResource;
}
}

// FIXME nasty public morozov
#define private friend class Wt::Wc::ReporterResource; private
#include <Wt/WMemoryResource>
#undef private
// FIXME http://redmine.emweb.be/issues/1210

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
        image_paths = boost::make_shared<AdBlockDetector::Strings>();
        i("/ad/side_");
        i("/adseo.");
        i("/adzone_");
        i("/external/ad/ad");
        i("/getmarketplaceads.");
        i("/popunderking.");
        i("/simpleadvert/ad");
        i("/top_ads/ad");
        i("_adwrap.");
        i("/160x600.");
    }

    void b(const std::string& url, const std::string& symbol) {
        banner_libs->push_back(std::make_pair(url, symbol));
    }

    void r(const std::string& url, const std::string& symbol) {
        regular_libs->push_back(std::make_pair(url, symbol));
    }

    void i(const std::string& part) {
        image_paths->push_back(part);
    }

    AdBlockDetector::LibsPtr banner_libs;
    AdBlockDetector::LibsPtr regular_libs;
    AdBlockDetector::StringsPtr image_paths;
} default_lists;

static const unsigned char gif1x1[] = {
    0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x01, 0x00, 0x01, 0x00,
    0x80, 0x00, 0x00, 0xdb, 0xdf, 0xef, 0x00, 0x00, 0x00, 0x21,
    0xf9, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x02, 0x02, 0x44,
    0x01, 0x00, 0x3b
};

class ReporterResource : public WMemoryResource {
public:
    ReporterResource(bool* flag, WObject* parent):
        WMemoryResource("image/gif", parent),
        flag_(flag) {
        setData(gif1x1, 43);
    }

    ~ReporterResource() {
        beingDeleted();
    }

protected:
    void handleRequest(const Http::Request& request, Http::Response& response) {
        *flag_ = true;
        WMemoryResource::handleRequest(request, response);
    }

private:
    bool* flag_;
};

AdBlockDetector::AdBlockDetector(WContainerWidget* parent, bool call_start):
    WContainerWidget(parent),
    signal_(this, "abdetector"),
    local_banner_image_(false),
    local_regular_image_(false),
    remote_banner_js_(false),
    remote_regular_js_(false),
    banner_ids_hidden_(false),
    banner_libs_(default_lists.banner_libs),
    regular_libs_(default_lists.regular_libs),
    image_paths_(default_lists.image_paths),
    banner_image_(new ReporterResource(&local_banner_image_, this)),
    regular_image_(new ReporterResource(&local_regular_image_, this)) {
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
    banner_image_->setInternalPath(image_path(/* banner */ true));
    regular_image_->setInternalPath(image_path(/* banner */ false));
    new WImage(banner_image_, /* alt text */ "", this);
    new WImage(regular_image_, /* alt text */ "", this);
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

std::string AdBlockDetector::image_path(bool banner) const {
    std::string result = "/" + WRandom::generateId(); // TODO length = rr(5,10)
    if (banner) {
        const Strings& parts = *image_paths_;
        result += parts[rr(parts.size())];
    } else {
        result += ".";
    }
    result += WRandom::generateId(); // TODO length = rr(2,4)
    return result;
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

