/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WTableView>
#include <Wt/WStringListModel>

#include <Wt/Wc/Pager.hpp>
#include <Wt/Wc/rand.hpp>
#include <Wt/Wc/util.hpp>

using namespace Wt;
using namespace Wt::Wc;

class TableView : public WTableView {
protected:
    WWidget* createPageNavigationBar() {
        return new Pager(this);
    }
};

class PagerApp : public WApplication {
public:
    PagerApp(const WEnvironment& env):
        WApplication(env) {
        if (env.ajax()) {
            root()->addWidget(new WText("Turn off JavaScript to try this"));
            root()->addWidget(new WBreak());
        }
        WStringListModel* model = new WStringListModel(this);
        int count = rr(100, 10000);
        for (int i = 0; i < count; i++) {
            model->addString(TO_S(i));
        }
        TableView* view = new TableView;
        view->setModel(model);
        view->resize(500, 500);
        root()->addWidget(view);
        scroll_to_last(view);
    }
};

WApplication* createPagerApp(const WEnvironment& env) {
    return new PagerApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createPagerApp);
}

