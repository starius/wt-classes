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
#include <Wt/WLineEdit>
#include <Wt/WContainerWidget>
#include <Wt/Wc/Recaptcha.hpp>

using namespace Wt;
using namespace Wt::Wc;

class RecaptchaApp : public WApplication {
public:
    RecaptchaApp(const WEnvironment& env):
        WApplication(env) {
        messageResourceBundle().use(Wt::WApplication::appRoot() +
                                    "locales/wtclasses");
        public_key_ = new WLineEdit("public key", root());
        private_key_ = new WLineEdit("private key", root());
        WPushButton* start = new WPushButton("Start", root());
        start->clicked().connect(this, &RecaptchaApp::show_recaptcha);
    }

    void show_recaptcha() {
        AbstractCaptcha* captcha = new Recaptcha(public_key_->text().toUTF8(),
                private_key_->text().toUTF8());
        root()->clear();
        root()->addWidget(captcha);
        WPushButton* check = new WPushButton("Check", root());
        WText* result = new WText(root());
        check->clicked().connect(captcha, &AbstractCaptcha::check);
        captcha->solved().connect(boost::bind(&WText::setText, result, "Ok"));
    }

private:
    WLineEdit* public_key_;
    WLineEdit* private_key_;
};

WApplication* createRecaptchaApp(const WEnvironment& env) {
    return new RecaptchaApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createRecaptchaApp);
}

