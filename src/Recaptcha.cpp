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
                     "{theme: 'custom',"
                     "callback: function() {" +
                     "$(" + challenge_field_->jsRef() + ")"
                     ".val(Recaptcha.get_challenge());"
                     "}});");
        WPushButton* u = new WPushButton(tr("wc.common.Update"), get_impl());
        u->clicked().connect(this, &AbstractCaptcha::update);
    }
}

// TODO:
// HTML version

// TODO:
// While theming does give you many options, you need to follow some user interface consistency rules:
// You must state that you are using reCAPTCHA near the CAPTCHA widget.
// You must provide a visible button that calls the reload function.
// You must provide a way for visually impaired users to access an audio CAPTCHA.
// You must provide alt text for any images that you use as buttons in the reCAPTCHA widget.

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

