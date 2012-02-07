/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cstdlib>
#include <boost/lexical_cast.hpp>

#include <Wt/WApplication>
#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WRandom>
#include <Wt/Wc/Url.hpp>

using namespace Wt;
using namespace Wt::Wc::url;

class App : public WApplication {
public:
    App(const WEnvironment& env):
        WApplication(env) {
        parser_ = new Parser(this);
        about_ = new PredefinedNode("about", parser_);
        about_smth_ = new StringNode(about_);
        users_ = new PredefinedNode("user", parser_);
        user_profile_ = new IntegerNode(users_);
        //
        parser_->opened().connect(this, &App::show_main);
        about_->opened().connect(this, &App::show_about);
        about_smth_->opened().connect(this, &App::show_about_smth);
        users_->opened().connect(this, &App::show_users);
        user_profile_->opened().connect(this, &App::show_user_profile);
        //
        WAnchor* main = new WAnchor(parser_->link(), root());
        main->setText("Main page");
        contents_ = new WContainerWidget(root());
        //
        internalPathChanged().connect(parser_, &Parser::open);
        parser_->open(internalPath());
    }

    void show_main() {
        contents_->clear();
        WAnchor* about = new WAnchor(about_->link(), contents_);
        about->setText(about_->full_path());
        new WBreak(contents_);
        WAnchor* users = new WAnchor(users_->link(), contents_);
        users->setText(users_->full_path());
    }

    void show_about() {
        contents_->clear();
        new WText("Items:", contents_);
        for (int i = 0; i < 10; ++i) {
            new WBreak(contents_);
            std::string id = Wt::WRandom::generateId();
            WAnchor* smth = new WAnchor(about_smth_->get_link(id), contents_);
            smth->setText(about_smth_->get_full_path(id));
        }
    }

    void show_about_smth() {
        contents_->clear();
        std::string id = about_smth_->value();
        new WText("Page about " + id, contents_);
    }

    void show_users() {
        contents_->clear();
        new WText("Items:", contents_);
        for (int i = 0; i < 10; ++i) {
            new WBreak(contents_);
            int id = abs(Wt::WRandom::get());
            WAnchor* user = new WAnchor(user_profile_->get_link(id), contents_);
            user->setText(user_profile_->get_full_path(id));
        }
    }

    void show_user_profile() {
        contents_->clear();
        int user_id = user_profile_->integer();
        std::string user_id_str = boost::lexical_cast<std::string>(user_id);
        new WText("User " + user_id_str, contents_);
    }

private:
    WContainerWidget* contents_;
    Parser* parser_;
    PredefinedNode* about_;
    StringNode* about_smth_;
    PredefinedNode* users_;
    IntegerNode* user_profile_;
};

WApplication* createApplication(const WEnvironment& env) {
    return new App(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createApplication);
}

