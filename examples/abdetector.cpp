/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>
#include <Wt/WTimer>
#include <Wt/WText>
#include <Wt/Wc/AdBlockDetector.hpp>

using namespace Wt;
using namespace Wt::Wc;

class AdBlockDetectorApp : public WApplication {
public:
    AdBlockDetectorApp(const WEnvironment& env):
        WApplication(env) {
        text_ = new WText("Please wait...", root());
        detector_ = new AdBlockDetector(root());
        WTimer::singleShot(10 * 1000, this, &AdBlockDetectorApp::find_adblock);
    }

    void find_adblock() {
        text_->setText(detector_->has_adblock() ?
                       "AdBlock detected" : "No AdBlock detected");
        delete detector_;
        detector_ = 0;
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

