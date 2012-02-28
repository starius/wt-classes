/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include <Wt/WApplication>
#include <Wt/WLineEdit>
#include <Wt/WSlider>
#include <Wt/WButtonGroup>
#include <Wt/WRadioButton>
#include <Wt/WCheckBox>
#include <Wt/WBreak>
#include <Wt/Wc/GravatarImage.hpp>

using namespace Wt;
using namespace Wt::Wc;

void set_email(GravatarImage* gravatar, WLineEdit* email) {
    gravatar->set_email(email->text().toUTF8());
}

void set_size(GravatarImage* gravatar, WSlider* size) {
    gravatar->set_size(size->value());
}

void set_default(GravatarImage* gravatar, WButtonGroup* builtin,
                 WLineEdit* custom_url) {
    if (builtin->checkedId() == 9000) {
        gravatar->set_default(custom_url->text().toUTF8());
    } else {
        gravatar->set_default(GravatarImage::BuiltIn(builtin->checkedId()));
    }
}

void set_rating(GravatarImage* gravatar, WButtonGroup* rating) {
    gravatar->set_rating(GravatarImage::Rating(rating->checkedId()));
}

class GravatarApp : public WApplication {
public:
    GravatarApp(const WEnvironment& env):
        WApplication(env) {
        GravatarImage* gravatar = new GravatarImage("test@test.com", root());
        new WBreak(root());
        WLineEdit* email = new WLineEdit(root());
        email->changed().connect(boost::bind(set_email, gravatar, email));
        email->setText("test@test.com");
        new WBreak(root());
        WSlider* size = new WSlider(root());
        size->setRange(1, 512);
        size->setValue(80);
        size->valueChanged().connect(boost::bind(set_size, gravatar, size));
        new WBreak(root());
        WButtonGroup* builtin = new WButtonGroup(this);
        builtin->addButton(new WRadioButton("default", root()),
                           GravatarImage::DEFAULT);
        builtin->addButton(new WRadioButton("404", root()),
                           GravatarImage::RETURN_404);
        builtin->addButton(new WRadioButton("mm", root()),
                           GravatarImage::MM);
        builtin->addButton(new WRadioButton("identicon", root()),
                           GravatarImage::IDENTICON);
        builtin->addButton(new WRadioButton("monsterid", root()),
                           GravatarImage::MONSTERID);
        builtin->addButton(new WRadioButton("wavatar", root()),
                           GravatarImage::WAVATAR);
        builtin->addButton(new WRadioButton("retro", root()),
                           GravatarImage::RETRO);
        builtin->addButton(new WRadioButton("custom url", root()),
                           9000);
        WLineEdit* custom_url = new WLineEdit(root());
        custom_url->setText("http://www.google.com/favicon.ico");
        builtin->setCheckedButton(builtin->button(GravatarImage::DEFAULT));
        builtin->checkedChanged().connect(boost::bind(set_default,
                                          gravatar, builtin, custom_url));
        new WBreak(root());
        WButtonGroup* rating = new WButtonGroup(this);
        rating->addButton(new WRadioButton("G", root()), GravatarImage::G);
        rating->addButton(new WRadioButton("PG", root()), GravatarImage::PG);
        rating->addButton(new WRadioButton("R", root()), GravatarImage::R);
        rating->addButton(new WRadioButton("X", root()), GravatarImage::X);
        rating->setCheckedButton(rating->button(GravatarImage::G));
        rating->checkedChanged().connect(boost::bind(set_rating,
                                         gravatar, rating));
        new WBreak(root());
        WCheckBox* fd = new WCheckBox("Force default", root());
        fd->checked().connect(boost::bind(&GravatarImage::set_force_default,
                                          gravatar, true));
        fd->unChecked().connect(boost::bind(&GravatarImage::set_force_default,
                                            gravatar, false));
        // TODO: set_secure_requests
    }
};

WApplication* createGravatarApp(const WEnvironment& env) {
    return new GravatarApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createGravatarApp);
}

