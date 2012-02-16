/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WServer>
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/Http/Client>
#include <Wt/Http/Message>

#include "Recaptcha.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

Recaptcha::Recaptcha(const std::string& public_key,
                     const std::string& private_key,
                     WContainerWidget* parent):
    AbstractCaptcha(parent),
    public_key_(public_key),
    private_key_(private_key) {
    wApp->enableUpdates();
    wApp->require("http://www.google.com/recaptcha/api/js/recaptcha_ajax.js",
                  "Recaptcha");
    http_ = new Http::Client(this);
    http_->done().connect(this, &Recaptcha::http_done);
    update_impl();
}

Recaptcha::~Recaptcha() {
    doJavaScript("Recaptcha.destroy();");
    doJavaScript("clearTimeout($(" + jsRef() + ").data('timer'));");
}

void Recaptcha::update_impl() {
    if (!implementation()) {
        setImplementation(new WContainerWidget());
    }
    get_impl()->clear();
    new WText("reCAPTCHA", get_impl());
    if (js()) {
        WContainerWidget* image = new WContainerWidget(get_impl());
        image->setId("recaptcha_image");
        response_field_ = new WLineEdit(get_impl());
        challenge_field_ = new WLineEdit(get_impl());
        // not challenge_field_->hide() to get its .text()
        doJavaScript("$(" + challenge_field_->jsRef() + ").hide();");
        response_field_->setId("recaptcha_response_field");
        doJavaScript("Recaptcha.create('" + public_key_  + "', '',"
                     "{theme: 'custom'});");
        WPushButton* u = new WPushButton(tr("wc.common.Update"), get_impl());
        u->clicked().connect(this, &AbstractCaptcha::update);
        WPushButton* get_image = new WPushButton(get_impl());
        get_image->addStyleClass("recaptcha_only_if_audio");
        get_image->setText(tr("wc.captcha.Get_image"));
        get_image->clicked().connect("function() {"
                                     "Recaptcha.switch_type('image') }");
        WPushButton* get_audio = new WPushButton(get_impl());
        get_audio->addStyleClass("recaptcha_only_if_image");
        get_audio->setText(tr("wc.captcha.Get_audio"));
        get_audio->clicked().connect("function() {"
                                     "Recaptcha.switch_type('audio') }");
        doJavaScript("clearTimeout($(" + jsRef() + ").data('timer'));");
        doJavaScript("$(" + jsRef() + ").data('timer',"
                     "setInterval(function() {"
                     "$(" + challenge_field_->jsRef() + ")"
                     ".val(Recaptcha.get_challenge());"
                     "}, 200));");
    }
}

// TODO:
// HTML version

void Recaptcha::check_impl() {
    if (js()) {
        std::string challenge = challenge_field_->text().toUTF8();
        std::string response = response_field_->text().toUTF8();
        boost::replace_all(response, " ", "+"); // TODO url encode
        const std::string& remoteip = wApp->environment().clientAddress();
        Http::Message m;
        m.setHeader("Content-Type", "application/x-www-form-urlencoded");
        m.addBodyText("privatekey=" + private_key_ + "&");
        m.addBodyText("remoteip=" + remoteip + "&");
        m.addBodyText("challenge=" + challenge + "&"); // TODO url encode
        m.addBodyText("response=" + response + "&"); // TODO url encode
        if (!http_->post("http://www.google.com/recaptcha/api/verify", m)) {
            update();
        }
    }
}

bool Recaptcha::js() const {
    return wApp->environment().javaScript();
}

WContainerWidget* Recaptcha::get_impl() {
    return downcast<WContainerWidget*>(implementation());
}

void Recaptcha::http_done(const boost::system::error_code& e,
                          const Http::Message& response) {
    if (!e && boost::starts_with(response.body(), "true")) {
        solve();
    } else {
        update();
    }
    updates_poster(WServer::instance(), wApp);
}

}

}

