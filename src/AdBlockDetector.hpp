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

    /** Constructor.
    \param parent Parent widget.
    \param call_start If the widgets should be automatically start().
        If you want to change some parameters (i.e., set_banner_libs()),
        this argument must be set to false,
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
     - http://js.adscale.de/getads.js (var adscale)
     - http://view.binlayer.com/ad-00000.js (var blLayer)
     - http://ads.adtiger.de/adscript.php (var adspirit_pid)
     - http://pagead2.googlesyndication.com/pagead/show_ads.js
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
    http://cdnjs.cloudflare.com/ajax/libs/underscore.js/1.3.1/underscore-min.js
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

    /** Create widgets, needed for the check.
    You don't need to call this method,
    if AdBlockDetector() was constructed with call_start=true.
    Otherwise, this method must be called after any configuration was done.
    */
    void start();

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

private:
    JSignal<std::string> signal_;
    bool local_banner_image_ : 1;
    bool local_regular_image_ : 1;
    bool remote_banner_js_ : 1;
    bool remote_regular_js_ : 1;
    bool banner_ids_hidden_ : 1;
    LibsPtr banner_libs_;
    LibsPtr regular_libs_;
    // TODO: hidden ids, images

    void signal_handler(std::string name);

    void check_remote_js(bool banner);
};

}

}

#endif

