/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/Wc/PaintedCaptcha.hpp>

using namespace Wt;
using namespace Wt::Wc;

class CaptchaApp : public WApplication {
public:
    CaptchaApp(const WEnvironment& env):
        WApplication(env) {
        captcha_ = new PaintedCaptcha(root());
        WPushButton* check = new WPushButton("Check", root());
        result_ = new WText(root());
        check->clicked().connect(captcha_, &AbstractCaptcha::check);
        captcha_->solved().connect(this, &CaptchaApp::solve_captcha);
    }

private:
    AbstractCaptcha* captcha_;
    WText* result_;

    void solve_captcha() {
        result_->setText("Ok");
    }
};

WApplication* createCaptchaApp(const WEnvironment& env) {
    return new CaptchaApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createCaptchaApp);
}

