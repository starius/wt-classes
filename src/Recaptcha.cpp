/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "config.hpp"
#include "global.hpp"

#include <boost/algorithm/string/predicate.hpp>

#include <Wt/WConfig.h>
#include <Wt/WServer>
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WContainerWidget>
#include <Wt/WTemplate>
#include <Wt/WLineEdit>
#include <Wt/WTextArea>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/Http/Client>
#include <Wt/Http/Message>

#ifndef WC_HAVE_WCOMPOSITEWIDGET_IMPLEMENTATION
// FIXME nasty public morozov
#define private friend class Wt::Wc::Recaptcha; private
#include <Wt/WCompositeWidget>
#undef private
#define implementation() Wt::WCompositeWidget::impl_
#endif // WC_HAVE_WCOMPOSITEWIDGET_IMPLEMENTATION

#include "Recaptcha.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

Recaptcha::Recaptcha(const std::string& public_key,
                     const std::string& private_key,
                     WContainerWidget* parent):
    AbstractCaptcha(parent),
    buttons_enabled_(true),
    public_key_(public_key),
    private_key_(private_key),
    input_(0),
    response_field_(0),
    challenge_field_(0) {
    wApp->enableUpdates();
    wApp->require("https://www.google.com/recaptcha/api/js/recaptcha_ajax.js",
                  "Recaptcha");
    http_ = new Http::Client(this);
    http_->done().connect(this, &Recaptcha::http_done);
    update_impl();
}

Recaptcha::~Recaptcha() {
    doJavaScript("Recaptcha.destroy();");
    doJavaScript("clearTimeout($(" + jsRef() + ").data('timer'));");
}

void Recaptcha::get_image() {
    doJavaScript("Recaptcha.switch_type('image');");
}

void Recaptcha::get_audio() {
    doJavaScript("Recaptcha.switch_type('audio');");
}

void Recaptcha::set_buttons(bool enabled) {
    buttons_enabled_ = enabled;
    update();
}

void Recaptcha::set_input(WFormWidget* input) {
    input_ = input;
    update();
}

void Recaptcha::update_impl() {
    if (!implementation()) {
        setImplementation(new WContainerWidget());
    }
    get_impl()->clear();
    WText* title = new WText("reCAPTCHA", get_impl());
    title->addStyleClass("wc_recaptcha_title");
    if (js()) {
        WContainerWidget* image = new WContainerWidget(get_impl());
        image->setId("recaptcha_image");
        response_field_ = input_ ? input_ : new WLineEdit(get_impl());
        challenge_field_ = new WLineEdit(get_impl());
        // not challenge_field_->hide() to get its .text()
        doJavaScript("$(" + challenge_field_->jsRef() + ").hide();");
        response_field_->setId("recaptcha_response_field");
        doJavaScript("Recaptcha.create('" + public_key_  + "', '',"
                     "{theme: 'custom'});");
        if (buttons_enabled_) {
            add_buttons();
        }
        doJavaScript("clearTimeout($(" + jsRef() + ").data('timer'));");
        doJavaScript("$(" + jsRef() + ").data('timer',"
                     "setInterval(function() {"
                     "$(" + challenge_field_->jsRef() + ")"
                     ".val(Recaptcha.get_challenge());"
                     "}, 200));");
    } else {
        WTemplate* iframe = new WTemplate(get_impl());
        iframe->setTemplateText("<iframe src='https://www.google.com/recaptcha/"
                                "api/noscript?k=" + public_key_ +
                                "' height='300' width='500' frameborder='0'>"
                                "</iframe>", XHTMLUnsafeText);
        if (input_) {
            challenge_field_ = input_;
        } else {
            WTextArea* ta = new WTextArea(get_impl());
            ta->setColumns(40);
            ta->setRows(3);
            challenge_field_ = ta;
        }
        response_field_ = new WLineEdit("manual_challenge", get_impl());
        response_field_->hide();
    }
}

void Recaptcha::check_impl() {
    std::string challenge = value_text(challenge_field_).toUTF8();
    std::string response = value_text(response_field_).toUTF8();
    const std::string& remoteip = wApp->environment().clientAddress();
    Http::Message m;
    m.setHeader("Content-Type", "application/x-www-form-urlencoded");
    m.addBodyText("privatekey=" + private_key_ + "&");
    m.addBodyText("remoteip=" + remoteip + "&");
    m.addBodyText("challenge=" + urlencode(challenge) + "&");
    m.addBodyText("response=" + urlencode(response) + "&");
#ifdef WT_WITH_SSL
    std::string schema = "https";
#else
    std::string schema = "http";
#endif
    if (!http_->post(schema + "://www.google.com/recaptcha/api/verify", m)) {
        mistake(tr("wc.captcha.Internal_error"));
    }
}

bool Recaptcha::js() const {
    return wApp->environment().javaScript();
}

WContainerWidget* Recaptcha::get_impl() {
    return DOWNCAST<WContainerWidget*>(implementation());
}

void Recaptcha::http_done(const boost::system::error_code& e,
                          const Http::Message& response) {
    if (e) {
        mistake(tr("wc.captcha.Internal_error"));
    } else if (boost::starts_with(response.body(), "true")) {
        solve();
    } else if (boost::contains(response.body(), "incorrect-captcha-sol")) {
        mistake(tr("wc.captcha.Wrong_response"));
    } else {
        mistake(tr("wc.captcha.Internal_error"));
    }
    updates_poster(WServer::instance(), wApp);
}

void Recaptcha::add_buttons() {
    WPushButton* u = new WPushButton(tr("wc.common.Update"), get_impl());
    u->clicked().connect(this, &AbstractCaptcha::update);
    WPushButton* get_image = new WPushButton(get_impl());
    get_image->addStyleClass("recaptcha_only_if_audio");
    get_image->setText(tr("wc.captcha.Get_image"));
    get_image->clicked().connect(this, &Recaptcha::get_image);
    WPushButton* get_audio = new WPushButton(get_impl());
    get_audio->addStyleClass("recaptcha_only_if_image");
    get_audio->setText(tr("wc.captcha.Get_audio"));
    get_audio->clicked().connect(this, &Recaptcha::get_audio);
}

}

}

