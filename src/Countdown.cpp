/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <ctype.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WDateTime>
#include <Wt/WViewWidget>
#include <Wt/WText>

#include "Countdown.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

using namespace td;

class Countdown::View : public WViewWidget {
public:
    View(Countdown* parent):
        WViewWidget(parent)
    { }

protected:
    WWidget* renderView() {
        return new WText(countdown()->current_text());
    }

private:
    Countdown* countdown() {
        return downcast<Countdown*>(parent());
    }
};

Countdown::Countdown(WContainerWidget* parent):
    WContainerWidget(parent),
    view_(0),
    js_(wApp->environment().javaScript()),
    since_(now()) {
    setInline(true);
    wApp->require(config_value("resourcesURL", "resources/") +
                  "Wc/js/jquery.countdown.js");
    doJavaScript("$(" + jsRef() + ").countdown({since: 0, compact: true});");
    set_format();
    set_time_separator();
    if (!js_) {
        view_ = new View(this);
    }
}

void Countdown::set_since(const WDateTime& since) {
    set_since(since - now());
}

void Countdown::set_since(const TimeDuration& since) {
    change("since", duration_for_js(since));
    change("until", "null");
    since_ = now() + since;
    until_ = WDateTime();
    update_view();
}

void Countdown::set_until(const WDateTime& until) {
    set_until(until - now());
}

void Countdown::set_until(const TimeDuration& until) {
    change("until", duration_for_js(until));
    change("since", "null");
    until_ = now() + until;
    since_ = WDateTime();
    update_view();
}

void Countdown::set_format(const std::string& format) {
    change("format", format, /* stringify_value */ true);
    format_ = format;
    update_view();
}

void Countdown::set_time_separator(const std::string& time_separator) {
    change("timeSeparator", time_separator, /* stringify_value */ true);
    time_separator_ = time_separator;
    update_view();
}

void Countdown::change(const std::string& name, const std::string& value,
                       bool stringify_value) {
    if (js_) {
        doJavaScript("$(" + jsRef() + ")"
                     ".countdown('change', '" + name + "', " +
                     (stringify_value ? stringify(value) : value) + ");");
    }
}

const char PERIOD_LETTERS[] = "YOWDHMS";
const TimeDuration PERIOD_DURATIONS[] = {
    365 * DAY, 30 * DAY, WEEK, DAY, HOUR, MINUTE, SECOND
};
const int PERIOD_LENGTH = 7;
const char* const COMPACT_LABELS = "ymwd";
const int COMPACT_LABELS_LENGTH = 4;

std::string Countdown::current_text() const {
    TimeDuration remaining_duration = current_duration();
    std::string result;
    int i = 0;
    BOOST_FOREACH (char p, format_) {
        while (toupper(p) != PERIOD_LETTERS[i] && i != PERIOD_LENGTH) {
            ++i;
        }
        if (i != PERIOD_LENGTH) {
            int c = remaining_duration / PERIOD_DURATIONS[i];
            bool uppercase = PERIOD_LETTERS[i] == p;
            if (c || uppercase) {
                if (!result.empty() && isalpha(result[result.size() - 1])) {
                    result += ' ';
                }
                if (i < COMPACT_LABELS_LENGTH) {
                    result += TO_S(c) + COMPACT_LABELS[i];
                } else {
                    if (!result.empty() && isdigit(result[result.size() - 1])) {
                        result += time_separator_;
                    }
                    result += str(boost::format("%02i") % c);
                }
                remaining_duration -= c * PERIOD_DURATIONS[i];
            }
        }
    }
    return result;
}

TimeDuration Countdown::current_duration() const {
    TimeDuration r =  since_.isNull() ? until_ - now() : now() - since();
    if (r.is_negative()) {
        r = 0 * SECOND;
    }
    return r;
}

std::string Countdown::duration_for_js(const TimeDuration& duration) {
    return TO_S(duration.total_nanoseconds()) + "/1.e9";
}

void Countdown::update_view() {
    if (view_) {
        view_->update();
    }
}

}

}

