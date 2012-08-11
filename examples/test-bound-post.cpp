/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cassert>
#include <iostream>

#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WText>
#include <Wt/Wc/TimeDuration.hpp>
#include <Wt/Wc/util.hpp>

using namespace Wt;
using namespace Wt::Wc;

void kill_app() {
    assert(wApp);
    assert(wApp->root());
    wApp->log("notice") << "test_app()";
    wApp->quit();
}

void do_this_in_background() {
    assert(!wApp);
    std::cerr << "bound_post() works well if wApp == 0" << std::endl;
}

void background_function() {
    assert(!wApp);
    bound_post(do_this_in_background)();
}

void go_to_background() {
    schedule_action(td::TD_NULL, background_function);
}

class BoundPostApp : public WApplication {
public:
    BoundPostApp(const WEnvironment& env):
        WApplication(env) {
        new WText("This application checks bound_post() method ", root());
        new WText("(internal check)", root());
        for (int i = 0; i < 50; i++) {
            schedule_action(td::rand_range(td::SECOND, 5 * td::SECOND),
                            bound_post(kill_app));
        }
        go_to_background();
    }
};

WApplication* createBoundPostApp(const WEnvironment& env) {
    return new BoundPostApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createBoundPostApp);
}

