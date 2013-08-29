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
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/Wc/GlobalLocalizedStrings.hpp>
#include <Wt/Wc/util.hpp>

using namespace Wt;
using namespace Wt::Wc;

GlobalLocalizedStrings gls;

class GlsApp : public WApplication {
public:
    GlsApp(const WEnvironment& env):
        WApplication(env) {
        setLocalizedStrings(gls.create_localized_strings());
        current_locale_ = new WText(Wc::get_locale());
        root()->addWidget(current_locale_);
        add_text("wc.wbi.View");
        add_text("Wt.WDate.Monday");
        add_button("en");
        add_button("ru");
        add_button("uk");
        add_button("be");
    }

private:
    WText* current_locale_;

    void add_text(const char* text) {
        root()->addWidget(new WBreak);
        root()->addWidget(new WText(tr(text)));
    }

    void add_button(std::string lang) {
        root()->addWidget(new WBreak);
        Wt::WPushButton* change_lang = new WPushButton(lang);
        change_lang->clicked().connect(boost::bind(&GlsApp::set_lang,
                                       this, lang));
        root()->addWidget(change_lang);
    }

    void set_lang(std::string lang) {
        Wc::set_locale(lang);
        current_locale_->setText(lang);
    }

    WString tr(const char* text) {
        return WString::tr(text);
    }
};

WApplication* createGlsApp(const WEnvironment& env) {
    return new GlsApp(env);
}

struct ServerConfigGLS {
    ServerConfigGLS() {
        gls.use("locales/wtclasses");
        gls.add_lang("ru");
        gls.add_lang("be");
        gls.add_lang("uk");
    }
} server_config_gls;

int main(int argc, char** argv) {
    return WRun(argc, argv, &createGlsApp);
}

