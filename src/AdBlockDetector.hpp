/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_ADBLOCK_DETECTOR_HPP_
#define WC_ADBLOCK_DETECTOR_HPP_

#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>
#include <Wt/WJavaScript>

namespace Wt {

namespace Wc {

/** An invisible widget detecting ads blocking software usage.
There are browser addons (e.g., AdBlock), preventing web sites from showing ads.
Ads is hidden or even not loaded.
This widget was written to test, if such software is being used.
It is up to you how to use such an information.

This widget performs two tests if smth is loaded:
 - local image, containing ads-like part of URL,
 - load and execute remote JavaScript ads library (for Ajax sessions only).

In both cases, control tests are also performed.

Additional checks are applied:
 - If elements with some IDs have been hidden (for Ajax sessions only).

To turn off a check, use methods skip_remote_js(), skip_local_image(), etc.

This widget was inspired by http://antiblock.org

\ingroup protection
*/
class AdBlockDetector : public WContainerWidget {
public:
    /** Structure, keeping remote JavaScript URL and corresponding symbol */
    typedef std::pair<std::string, std::string> UrlAndSymbol;

    /** A collection of remote JavaScript libraries */
    typedef std::vector<UrlAndSymbol> Libs;

    /** A shared pointer to Libs */
    typedef boost::shared_ptr<Libs> LibsPtr;

    /** A collection of strings */
    typedef std::vector<std::string> Strings;

    /** A shared pointer to Strings */
    typedef boost::shared_ptr<Strings> StringsPtr;

    /** Constructor.
    \param parent Parent widget.
    \param call_start If the widgets should be automatically start().
        If you want to change some parameters (i.e., set_banner_libs(),
        skip_ids_hidden()), this argument must be set to false,
        and start() must be called explicitly afterwards.
    */
    AdBlockDetector(WContainerWidget* parent = 0, bool call_start = true);

    /** Get if the client uses ads blocking sowfware.
    \param true_if_maybe Consider ads blocking sowfware usage if not sure.

    You can call this method in 10 seconds after AdBlockDetector creation.
    */
    bool has_adblock(bool true_if_maybe = false) const;

    /** Get the collection of remote banner JavaScript libs.
    These libs are used by default set_js_params(banner=true).

    By default, the following libraries are used
    (this list is based on http://antiblock.org):
     - https://js.adscale.de/getads.js (var adscale)
     - https://ads.adtiger.de/adscript.php (var adspirit_pid)
     - https://pagead2.googlesyndication.com/pagead/show_ads.js
        (var google_ad_block).

    \attention This default list is shared across all AdBlockDetector instances.
        Should you change banner_libs() of one AdBlockDetector instance,
        all instances shall also be changed.
        Furthermore, if you should do it after application server start,
        it may result in races (and possible segmentation fault),
        as the same container would be written and read simultaneously.
        Use set_banner_libs() instead.
    */
    LibsPtr banner_libs() {
        return banner_libs_;
    }

    /** Set the collection of remote banner JavaScript libs */
    void set_banner_libs(LibsPtr banner_libs) {
        banner_libs_ = banner_libs;
    }

    /** Get the collection of remote regular (non-ads) JavaScript libs.
    These libs are used by default set_js_params(banner=false).

    By default, the following libraries are used:
    https://cdnjs.cloudflare.com/ajax/libs/underscore.js/1.3.1/underscore-min.js
        (var _)

    \attention See banner_libs().
    */
    LibsPtr regular_libs() {
        return regular_libs_;
    }

    /** Set the collection of remote regular (non-ads) JavaScript libs */
    void set_regular_libs(LibsPtr regular_libs) {
        regular_libs_ = regular_libs;
    }

    /** Get the collection of URL parts of local images checked.
    These image paths are used by default implementation of image_path().

    By default, the following path parts are used
    (this list is based on http://antiblock.org):
     - /ad/side_
     - /adseo.
     - /adzone_
     - /external/ad/ad
     - /getmarketplaceads.
     - /popunderking.
     - /simpleadvert/ad
     - /top_ads/ad
     - _adwrap.
     - /160x600.

    \attention See banner_libs().
    */
    StringsPtr image_paths() {
        return image_paths_;
    }

    /** Set the collection of URL parts of local images checked */
    void set_image_paths(StringsPtr image_paths) {
        image_paths_ = image_paths;
    }

    /** Get the collection of IDs of HTML elements to be checked.
    These IDs are used by default implementation of html_id().

    By default, the following IDs of HTML elements are used
    (this list is based on http://antiblock.org):
     - ad-sponsors
     - adBlock125
     - advertising-banner
     - home-rectangle-ad
     - priceGrabberAd
     - tmglBannerAd
     - topAdvBox
     - ad
     - ads
     - adsense

    \attention See banner_libs().
    */
    StringsPtr html_ids() {
        return html_ids_;
    }

    /** Set the collection of IDs of HTML elements to be checked */
    void set_html_ids(StringsPtr html_ids) {
        html_ids_ = html_ids;
    }

    /** Create widgets, needed for the check.
    You don't need to call this method,
    if AdBlockDetector() was constructed with call_start=true.
    Otherwise, this method must be called after any configuration was done.
    */
    void start();

    /** Skip the check that remote JS ads libs can be loaded */
    void skip_remote_js(bool skip = true) {
        skip_remote_js_ = skip;
    }

    /** Skip the check that local ads images can be loaded */
    void skip_local_image(bool skip = true) {
        skip_local_image_ = skip;
    }

    /** Skip the check that HTML elements with ads-like IDs are not hidden */
    void skip_ids_hidden(bool skip = true) {
        skip_ids_hidden_ = skip;
    }

protected:
    /** Set URL and var name to check existance.
    \param url URL of JavaScript library to load.
    \param symbol Variable name, which indicates successful library loading.
    \param banner If the library should be ads library or normal JS library.

    The default implementation of the method select random library from
    banner_libs() or regular_libs().
    Reimplementation of this method may be useful, if you get libraries
    in a complicated way, for example, from database.
    */
    virtual void set_js_params(std::string& url, std::string& symbol,
                               bool banner) const;

    /** Return local URL for image checked.
    \param banner If the image should be ads image or normal image.

    The default implementation of the method select random image from
    image_paths() for banner=true.
    The URL is prepended and appended with random strings.
    */
    virtual std::string image_path(bool banner) const;

    /** Return ID of HTML element to be checked for being hidden.
    \param banner If the ID should be ads ID or normal one.

    The default implementation of the method select random element from
    html_ids() for banner=true.
    For banner=false, random ID is returned.
    */
    virtual std::string html_id(bool banner) const;

private:
    JSignal<std::string> signal_;
    bool local_banner_image_;
    bool local_regular_image_;
    bool remote_banner_js_ : 1;
    bool remote_regular_js_ : 1;
    bool banner_ids_hidden_ : 1;
    bool regular_ids_hidden_ : 1;
    bool skip_remote_js_ : 1;
    bool skip_local_image_ : 1;
    bool skip_ids_hidden_ : 1;
    LibsPtr banner_libs_;
    LibsPtr regular_libs_;
    StringsPtr image_paths_;
    StringsPtr html_ids_;
    WResource* banner_image_;
    WResource* regular_image_;
    // TODO: hidden ids

    void signal_handler(std::string name);

    void check_remote_js(bool banner);
    void check_hidden(bool banner);
};

}

}

#endif

