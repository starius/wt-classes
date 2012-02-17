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
        WViewWidget(parent),
        since_(current_time())
    { }

    WDateTime since_;
    WDateTime until_;
    std::string format_;
    std::string time_separator_;
    WDateTime paused_;
    WDateTime lapped_;
    WDateTime resumed_;

    std::string current_text() const;

    td::TimeDuration current_duration() const;

protected:
    WWidget* renderView() {
        return new WText(current_text());
    }

private:
    Countdown* countdown() {
        return downcast<Countdown*>(parent());
    }

    WDateTime current_time() const {
        return const_cast<View*>(this)->countdown()->current_time();
    }
};

Countdown::Countdown(WContainerWidget* parent):
    WContainerWidget(parent),
    unit_(SECOND / 10),
    now_(now()),
    view_(0),
    expired_(0) {
    setInline(true);
    implementJavaScript(&Countdown::pause, wrap_js("'pause'"));
    implementJavaScript(&Countdown::lap, wrap_js("'lap'"));
    implementJavaScript(&Countdown::resume, wrap_js("'resume'"));
    wApp->require(config_value("resourcesURL", "resources/") +
                  "Wc/js/jquery.countdown.js");
    apply_js("{since: 0, compact: true}");
    if (!wApp->environment().javaScript()) {
        view_ = new View(this);
    }
    set_format();
    set_time_separator();
}

Countdown::~Countdown() {
    if (expired_) {
        delete expired_;
    }
}

void Countdown::set_since(const WDateTime& since) {
    set_since(since - current_time());
}

void Countdown::set_since(const TimeDuration& since) {
    change("since", duration_for_js(since));
    change("until", "null");
    if (view_) {
        view_->since_ = current_time() + since;
        view_->until_ = WDateTime();
        update_view();
    }
}

void Countdown::set_until(const WDateTime& until) {
    set_until(until - current_time());
}

void Countdown::set_until(const TimeDuration& until) {
    change("until", duration_for_js(until));
    change("since", "null");
    if (view_) {
        view_->until_ = current_time() + until;
        view_->since_ = WDateTime();
        update_view();
    }
}

void Countdown::set_format(const std::string& format) {
    change("format", format, /* stringify_value */ true);
    if (view_) {
        view_->format_ = format;
        update_view();
    }
}

void Countdown::set_time_separator(const std::string& time_separator) {
    change("timeSeparator", time_separator, /* stringify_value */ true);
    if (view_) {
        view_->time_separator_ = time_separator;
        update_view();
    }
}

void Countdown::change(const std::string& name, const std::string& value,
                       bool stringify_value) {
    apply_js("'change', '" + name + "', " +
             (stringify_value ? WWebWidget::jsStringLiteral(value) : value));
}

const char PERIOD_LETTERS[] = "YOWDHMS";
const TimeDuration PERIOD_DURATIONS[] = {
    365 * DAY, 30 * DAY, WEEK, DAY, HOUR, MINUTE, SECOND
};
const int PERIOD_LENGTH = 7;
const char* const COMPACT_LABELS = "ymwd";
const int COMPACT_LABELS_LENGTH = 4;

std::string Countdown::View::current_text() const {
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

TimeDuration Countdown::View::current_duration() const {
    WDateTime n = current_time();
    if (paused_.isValid() && paused_ <= current_time()) {
        n = paused_;
    } else if (lapped_.isValid() && lapped_ <= current_time()) {
        n = lapped_;
    } else if (resumed_.isValid() && resumed_ >= current_time()) {
        n = resumed_;
    }
    TimeDuration r = since_.isValid() ? n - since_ : until_ - n;
    if (r.is_negative()) {
        r = TD_NULL;
    }
    return r;
}

void Countdown::pause() {
    apply_js("'pause'");
    pause_html(TD_NULL);
}

void Countdown::pause(const td::TimeDuration& duration) {
    apply_js("'pause'", duration);
    pause_html(duration);
}

void Countdown::lap() {
    apply_js("'lap'");
    lap_html(TD_NULL);
}

void Countdown::lap(const td::TimeDuration& duration) {
    apply_js("'lap'", duration);
    lap_html(duration);
}

void Countdown::resume() {
    apply_js("'resume'");
    resume_html(TD_NULL);
}

void Countdown::resume(const td::TimeDuration& duration) {
    apply_js("'resume'", duration);
    resume_html(duration);
}

JSignal<>& Countdown::expired() {
    if (!expired_) {
        expired_ = new JSignal<>(this, "expired",
                                 /* collectSlotJavaScript */ true);
        change("onExpiry", "function() {" + expired_->createCall() + "}");
    }
    return *expired_;
}

WDateTime Countdown::current_time() const {
    WDateTime n = now();
    if (n - now_ > unit_) {
        now_ = n;
    }
    return now_;
}

std::string Countdown::duration_for_js(const TimeDuration& duration) {
    return TO_S(duration.total_nanoseconds()) + "/1.e9";
}

void Countdown::apply_js(const std::string& args) {
    if (!view_) {
        doJavaScript(wrap_js(args));
    }
}

void Countdown::apply_js(const std::string& args,
                         const td::TimeDuration& duration) {
    if (!view_) {
        doJavaScript("setTimeout(function() {" + wrap_js(args) +
                     "}, " + TO_S(duration.total_milliseconds()) + ");");
    }
}

std::string Countdown::wrap_js(const std::string& args) const {
    return "$(" + jsRef() + ").countdown(" + args + ");";
}

void Countdown::update_view() {
    if (view_) {
        view_->update();
    }
}

void Countdown::pause_html(const td::TimeDuration& duration) {
    if (view_) {
        view_->paused_ = current_time() + duration;
        view_->lapped_ = WDateTime();
        view_->resumed_ = WDateTime();
        update_view();
    }
}

void Countdown::lap_html(const td::TimeDuration& duration) {
    if (view_) {
        view_->paused_ = WDateTime() + duration;
        view_->lapped_ = current_time();
        view_->resumed_ = WDateTime();
        update_view();
    }
}

void Countdown::resume_html(const td::TimeDuration& duration) {
    if (view_) {
        if (view_->paused_.isValid()) {
            if (view_->since_.isValid()) {
                view_->since_ += current_time() + duration - view_->paused_;
            } else {
                view_->until_ -= current_time() + duration - view_->paused_;
            }
        }
        view_->paused_ = WDateTime();
        view_->lapped_ = WDateTime();
        view_->resumed_ = current_time() + duration;
        update_view();
    }
}

}

}

