/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

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
        messageResourceBundle().use(Wt::WApplication::appRoot() +
                                    "locales/wtclasses");
        AbstractCaptcha* captcha = new PaintedCaptcha(root());
        WPushButton* check = new WPushButton("Check", root());
        WText* result = new WText(root());
        check->clicked().connect(captcha, &AbstractCaptcha::check);
        captcha->solved().connect(boost::bind(&WText::setText, result, "Ok"));
        captcha->fault().connect(boost::bind(&WText::setText, result, _1));
        captcha->set_precheck(&AbstractCaptcha::frequency_check);
    }
};

WApplication* createCaptchaApp(const WEnvironment& env) {
    return new CaptchaApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createCaptchaApp);
}

