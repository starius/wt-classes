/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>
#include <Wt/WText>
#include <Wt/Wc/AdBlockDetector.hpp>
#include <Wt/Wc/TimeDuration.hpp>
#include <Wt/Wc/util.hpp>

using namespace Wt;
using namespace Wt::Wc;

class AdBlockDetectorApp : public WApplication {
public:
    AdBlockDetectorApp(const WEnvironment& env):
        WApplication(env) {
        wApp->enableUpdates();
        text_ = new WText("Please wait...", root());
        detector_ = new AdBlockDetector(root());
        Wc::schedule_action(10 * Wc::td::SECOND, bound_post(boost::bind(
                                &AdBlockDetectorApp::find_adblock, this)));
    }

    void find_adblock() {
        text_->setText(detector_->has_adblock() ?
                       "AdBlock detected" : "No AdBlock detected");
        delete detector_;
        detector_ = 0;
        wApp->triggerUpdate();
    }

private:
    AdBlockDetector* detector_;
    WText* text_;
};

WApplication* createAdBlockDetectorApp(const WEnvironment& env) {
    return new AdBlockDetectorApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createAdBlockDetectorApp);
}

