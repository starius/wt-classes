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
#include <Wt/Wc/Countdown.hpp>

using namespace Wt;
using namespace Wt::Wc;

class CountdownApp : public WApplication {
public:
    CountdownApp(const WEnvironment& env):
        WApplication(env) {
        new WText("Standard count-up: ", root());
        new Countdown(root());
        new WText(" from this application creation time", root());
        //
        new WBreak(root());
        new WText("Change time separator: ", root());
        Countdown* countdown = new Countdown(root());
        countdown->set_time_separator("-");
        WDateTime next_new_year(WDate(WDate::currentDate().year() + 1, 1, 1));
        countdown->set_until(next_new_year);
        new WText(" until new year (UTC)", root());
    }
};

WApplication* createApplication(const WEnvironment& env) {
    return new CountdownApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createApplication);
}

