/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WDate>
#include <Wt/WPushButton>
#include <Wt/Wc/Countdown.hpp>
#include <Wt/Wc/TimeDuration.hpp>
#include <Wt/Wc/TimeDurationWidget.hpp>
#include <Wt/Wc/util.hpp>

using namespace Wt;
using namespace Wt::Wc;
using namespace Wt::Wc::td;

class CountdownApp : public WApplication {
public:
    CountdownApp(const WEnvironment& env):
        WApplication(env) {
        messageResourceBundle().use(approot() + "locales/wtclasses");
        new WText("Standard count-up: ", root());
        Countdown* from_now = new Countdown(root());
        new WText(" from this application creation time", root());
        WPushButton* pause = new WPushButton("Pause", root());
        WPushButton* lap = new WPushButton("Lap", root());
        WPushButton* resume = new WPushButton("Resume", root());
        resume->hide();
        pause->clicked().connect(from_now, &Countdown::pause);
        pause->clicked().connect(pause, &WWidget::hide);
        pause->clicked().connect(lap, &WWidget::hide);
        pause->clicked().connect(resume, &WWidget::show);
        lap->clicked().connect(from_now, &Countdown::lap);
        lap->clicked().connect(lap, &WWidget::hide);
        lap->clicked().connect(pause, &WWidget::hide);
        lap->clicked().connect(resume, &WWidget::show);
        resume->clicked().connect(from_now, &Countdown::resume);
        resume->clicked().connect(resume, &WWidget::hide);
        resume->clicked().connect(pause, &WWidget::show);
        resume->clicked().connect(lap, &WWidget::show);
        //
        new WBreak(root());
        new WText("Change time separator: ", root());
        Countdown* countdown = new Countdown(root());
        countdown->set_time_separator("-");
        WDateTime next_new_year(WDate(WDate::currentDate().year() + 1, 1, 1));
        countdown->set_until(next_new_year);
        new WText(" until new year (UTC)", root());
        //
        new WBreak(root());
        new WText("Countdown and count-up, connected through JS: ", root());
        Countdown* a = new Countdown(root());
        a->set_until(5 * SECOND);
        new WText(" and ", root());
        Countdown* b = new Countdown(root());
        b->pause();
        a->expired()->connect(b, &Countdown::resume);
        //
        new WBreak(root());
        new WText("Count-up, paused in 10 s: ", root());
        Countdown* five = new Countdown(root());
        five->pause(10 * SECOND);
        //
        new WBreak(root());
        new WText("Already paused countdown ", root());
        Countdown* paused_countdown = new Countdown(root());
        paused_countdown->pause(); // before set_until()
        paused_countdown->set_until(SECOND);
        new WText(" and ", root());
        Countdown* paused_countdown2 = new Countdown(root());
        paused_countdown2->set_until(SECOND);
        paused_countdown2->pause(); // after set_until()
        new WText(" and already paused count-up ", root());
        Countdown* paused_countup = new Countdown(root());
        paused_countup->set_since(-SECOND);
        paused_countup->pause(); // after set_since()
        new WText(" and ", root());
        Countdown* paused_countup2 = new Countdown(root());
        paused_countup2->pause(); // before set_since()
        paused_countup2->set_since(-SECOND);
        new WText(". They should display 1 s constantly.", root());
        //
        new WBreak(root());
        new WText("Editable count-down: ", root());
        Countdown* editable = new Countdown(root());
        editable->set_until(TD_NULL);
        TimeDurationWidget* td_edit = new TimeDurationWidget(TD_NULL, DAY,
                1000 * DAY, root());
        WPushButton* set_td = new WPushButton("Set time duration", root());
        void (Countdown::*setter)(const TimeDuration&) = &Countdown::set_until;
        set_td->clicked().connect(
            boost::bind(setter, editable,
                        boost::bind(&TimeDurationWidget::corrected_value,
                                    td_edit)));
    }
};

WApplication* createCountdownApp(const WEnvironment& env) {
    return new CountdownApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createCountdownApp);
}

