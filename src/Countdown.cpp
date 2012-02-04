/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WDateTime>

#include "Countdown.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

using namespace td;

Countdown::Countdown(WContainerWidget* parent):
    WContainerWidget(parent) {
    setInline(true);
    wApp->require(config_value("resourcesURL", "resources/") +
                  "Wc/js/jquery.countdown.js");
    doJavaScript("$(" + jsRef() + ").countdown({since: 0, compact: true});");
}

void Countdown::set_since(const WDateTime& since) {
    set_since(since - now());
}

void Countdown::set_since(const TimeDuration& since) {
    change("since", duration_for_js(since));
    change("until", "null");
}

void Countdown::set_until(const WDateTime& until) {
    set_until(until - now());
}

void Countdown::set_until(const TimeDuration& until) {
    change("until", duration_for_js(until));
    change("since", "null");
}

void Countdown::set_format(const std::string& format) {
    change("format", format, /* stringify_value */ true);
}

void Countdown::set_time_separator(const std::string& time_separator) {
    change("timeSeparator", time_separator, /* stringify_value */ true);
}

void Countdown::change(const std::string& name, const std::string& value,
                       bool stringify_value) {
    if (wApp->environment().javaScript()) {
        doJavaScript("$(" + jsRef() + ")"
                     ".countdown('change', '" + name + "', " +
                     (stringify_value ? stringify(value) : value) + ");");
    }
}

std::string Countdown::duration_for_js(const TimeDuration& duration) {
    return TO_S(duration.total_nanoseconds()) + "/1.e9";
}

}

}

