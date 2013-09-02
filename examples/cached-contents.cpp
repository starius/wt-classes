/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <Wt/WApplication>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WAnchor>
#include <Wt/Wc/CachedContents.hpp>
#include <Wt/Wc/TimeDuration.hpp>
#include <Wt/Wc/Url.hpp>
#include <Wt/Wc/util.hpp>

using namespace Wt;
using namespace Wt::Wc;
using namespace Wt::Wc::url;
using namespace Wt::Wc::td;

class MyParser;
class MainWidget;

class CachedApp : public WApplication {
public:
    MyParser* parser;
    MainWidget* main_widget;

    CachedApp(const WEnvironment& env):
        WApplication(env) {
        root()->addWidget(new WText("Widgets cache size = 3"));
        root()->addWidget(new WBreak);
        anchor("/complex", "Root for widgets doing complex job");
        for (int i = 0; i < 10; i++) {
            anchor("/complex/" + TO_S(i), "Complex job " + TO_S(i));
        }
        anchor("/ignored-1", "Non-cached complex job");
        anchor("/ignored-2", "Root for non-cached complex job 2");
        anchor("/ignored-2/ignored-3", "Non-cached complex job 2");
    }

    void anchor(std::string url, WString text) {
        WAnchor* a = new WAnchor;
        a->setRefInternalPath(url);
        a->setText(text);
        root()->addWidget(a);
        root()->addWidget(new WBreak);
    }

    static CachedApp* instance() {
        return DOWNCAST<CachedApp*>(wApp);
    }
};

class MyParser : public Parser {
public:
    PredefinedNode* complex_root;
    IntegerNode* complex_node;
    PredefinedNode* ignored_node;
    PredefinedNode* ignored_tree;
    PredefinedNode* ignored_node_2;

    MyParser(WObject* parent):
        Parser(parent) {
        complex_root = new PredefinedNode("complex", this);
        complex_node = new IntegerNode(complex_root);
        ignored_node = new PredefinedNode("ignored-1", this);
        ignored_tree = new PredefinedNode("ignored-2", this);
        ignored_node_2 = new PredefinedNode("ignored-3", ignored_tree);
    }
};

class MainWidget : public CachedContents {
public:
    MainWidget(WContainerWidget* parent):
        CachedContents(parent) {
        set_cache_size(3);
        ignore_url("/ignored-1");
        ignore_prefix("/ignored-2");
    }

    void show_complex_root() {
        boost::this_thread::sleep(SECOND); // work
        set_contents_raw(new WText("Simulates CPU-expensive work"));
        wApp->setTitle("Simulates CPU-expensive work");
    }

    void show_complex(int id) {
        boost::this_thread::sleep(SECOND); // work
        set_contents_raw(new WText(TO_S(id)));
        wApp->setTitle(TO_S(id));
    }

    void show_ignored_node() {
        boost::this_thread::sleep(SECOND); // work
        set_contents_raw(new WText("This URL is not cached"));
        wApp->setTitle("This URL is not cached");
    }

    void show_ignored_prefix() {
        boost::this_thread::sleep(SECOND); // work
        set_contents_raw(new WText("This URL and sub-URLs are not cached"));
        wApp->setTitle("This URL and sub-URLs are not cached");
    }

    void show_ignored_node_2() {
        boost::this_thread::sleep(SECOND); // work
        set_contents_raw(new WText("This sub-URL is not cached"));
        wApp->setTitle("This sub-URL is not cached");
    }

protected:
    void open_url_impl(const std::string& url) {
        CachedApp::instance()->parser->open(url);
    }
};

void show_complex_node() {
    CachedApp* app = CachedApp::instance();
    int id = app->parser->complex_node->integer();
    app->main_widget->show_complex(id);
}

WApplication* createCachedApp(const WEnvironment& env) {
    CachedApp* app = new CachedApp(env);
    app->parser = new MyParser(app);
    app->main_widget = new MainWidget(app->root());
    MyParser* p = app->parser;
    MainWidget* m = app->main_widget;
    p->complex_root->opened().connect(m, &MainWidget::show_complex_root);
    p->complex_node->opened().connect(boost::bind(show_complex_node));
    p->ignored_node->opened().connect(m, &MainWidget::show_ignored_node);
    p->ignored_tree->opened().connect(m, &MainWidget::show_ignored_prefix);
    p->ignored_node_2->opened().connect(m, &MainWidget::show_ignored_node_2);
    app->internalPathChanged().connect(m, &CachedContents::open_url);
    m->open_url(app->internalPath());
    return app;
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createCachedApp);
}

