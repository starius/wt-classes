/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cstdlib>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include <Wt/WApplication>
#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <Wt/Wc/Url.hpp>
#include <Wt/Wc/rand.hpp>

using namespace Wt;
using namespace Wt::Wc;
using namespace Wt::Wc::url;

class UrlSitemapApp;

class MySiteMapGenerator : public SiteMapGenerator {
public:
    MySiteMapGenerator(Node* node, UrlSitemapApp* app):
        SiteMapGenerator(node), app_(app) {
        UrlParams params = default_params();
        params.changefreq = WEEKLY;
        set_default_params(params);
    }

protected:
    void for_each_value(Node* node, const AnyCaller& callback) const;

    bool node_handler(Node* node, UrlParams& params) const;

private:
    UrlSitemapApp* app_;
};

class UrlSitemapApp : public WApplication {
public:
    UrlSitemapApp(const WEnvironment& env):
        WApplication(env) {
        parser_ = new Parser(this);
        about_ = new PredefinedNode("about", parser_);
        about_smth_ = new StringNode(about_);
        users_ = new PredefinedNode("user", parser_);
        users_->set_slash_strategy(Node::IF_NOT_LAST);
        user_profile_ = new IntegerNode(users_);
        //
        // a) Node::opened()
        parser_->opened().connect(this, &UrlSitemapApp::show_main);
        about_->opened().connect(this, &UrlSitemapApp::show_about);
        about_smth_->opened().connect(this, &UrlSitemapApp::show_about_smth);
        //
        // b) Parser::connect
        parser_->connect(users_, boost::bind(&UrlSitemapApp::show_users, this));
        //
        // c) Parser::child_opened()
        parser_->child_opened().connect(this, &UrlSitemapApp::open_node);
        //
        WAnchor* main = new WAnchor(root());
        main->setRefInternalPath(parser_->full_path());
        main->setText("Main page");
        contents_ = new WContainerWidget(root());
        //
        internalPathChanged().connect(parser_, &Parser::open);
        parser_->open(internalPath());
    }

    void show_main() {
        contents_->clear();
        WAnchor* about = new WAnchor(contents_);
        about->setRefInternalPath(about_->full_path());
        about->setText(about_->full_path());
        new WBreak(contents_);
        WAnchor* users = new WAnchor(contents_);
        users->setRefInternalPath(users_->full_path());
        users->setText(users_->full_path());
        //
        show_sitemaps();
    }

    void show_sitemaps() {
        MySiteMapGenerator generator(parser_, this);
        std::stringstream ss;
        generator.generate(ss);
        new WBreak(contents_);
        new WText("General sitemap:", contents_);
        new WBreak(contents_);
        WTextArea* sitemap_textarea = new WTextArea(contents_);
        sitemap_textarea->setText(ss.str());
        sitemap_textarea->setRows(20);
        sitemap_textarea->setColumns(80);
        new WBreak(contents_);
        new WText("Sitemap of /about/", contents_);
        new WBreak(contents_);
        sitemap_textarea = new WTextArea(contents_);
        ss.str("");
        generator.set_root(about_);
        generator.generate(ss);
        sitemap_textarea->setText(ss.str());
        sitemap_textarea->setRows(20);
        sitemap_textarea->setColumns(80);
    }

    void show_about() {
        contents_->clear();
        new WText("Items:", contents_);
        for (int i = 0; i < 10; ++i) {
            new WBreak(contents_);
            std::string id = rand_string();
            WAnchor* smth = new WAnchor(contents_);
            smth->setRefInternalPath(about_smth_->get_full_path(id));
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
            int id = rr();
            WAnchor* user = new WAnchor(contents_);
            user->setRefInternalPath(user_profile_->get_full_path(id));
            user->setText(user_profile_->get_full_path(id));
        }
    }

    void show_user_profile() {
        contents_->clear();
        int user_id = user_profile_->integer();
        std::string user_id_str = boost::lexical_cast<std::string>(user_id);
        new WText("User " + user_id_str, contents_);
    }

    void open_node(Node* node) {
        if (node == user_profile_) {
            show_user_profile();
        }
    }

private:
    WContainerWidget* contents_;
    Parser* parser_;
    PredefinedNode* about_;
    StringNode* about_smth_;
    PredefinedNode* users_;
    IntegerNode* user_profile_;

    friend class MySiteMapGenerator;
};

void MySiteMapGenerator::for_each_value(Node* node,
                                        const AnyCaller& callback) const {
    if (node == app_->about_smth_) {
        callback(std::string("contacts"));
        callback(std::string("address"));
        callback(std::string("jobs"));
    } else if (node == app_->user_profile_) {
        callback(1);
        callback(2); // see bellow how this user can be excluded
        callback(3);
    }
}

bool MySiteMapGenerator::node_handler(Node* node, UrlParams& params) const {
    if (node == app_->parser_) {
        params.changefreq = HOURLY;
    }
    if (node == app_->about_smth_ && node->value() == "jobs") {
        params.priority = 0.9;
    }
    if (node == app_->user_profile_ && node->value() == "2") {
        return false; // exclude /user/2 from sitemap
    }
    return true;
}

WApplication* createUrlSitemapApp(const WEnvironment& env) {
    return new UrlSitemapApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createUrlSitemapApp);
}

