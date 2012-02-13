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
        check->clicked().connect(this, &CaptchaApp::check_captcha);
    }

private:
    AbstractCaptcha* captcha_;
    WText* result_;

    void check_captcha() {
        switch (captcha_->validate()) {
        case WValidator::Valid:
            result_->setText("Ok");
            break;
        case WValidator::InvalidEmpty:
            result_->setText("Empty");
            break;
        case WValidator::Invalid:
            result_->setText("Invalid");
            break;
        }
    }
};

WApplication* createCaptchaApp(const WEnvironment& env) {
    return new CaptchaApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createCaptchaApp);
}

