/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WPushButton>

#include <Wt/Wc/Notify.hpp>

using namespace Wt;
using namespace Wt::Wc;

notify::Server delete_server;

struct DeleteEvent : public notify::Event {
    std::string key() const {
        return "delete";
    }
};

class DeleteMeWidget : public WContainerWidget, notify::Widget {
public:
    DeleteMeWidget(const WString& name, WContainerWidget* parent):
        WContainerWidget(parent),
        notify::Widget(DeleteEvent().key(), &delete_server),
        name_(name) {
        addWidget(new WText(name));
    }

protected:
    void notify(notify::EventPtr /* event */) {
        wApp->root()->addWidget(new WText(name_ + " deleted"));
        delete this;
        updates_trigger();
    }

private:
    WString name_;
};

class NotifyDeleteApp : public WApplication {
public:
    NotifyDeleteApp(const WEnvironment& env):
        WApplication(env) {
        enableUpdates();
        parent_ = new DeleteMeWidget("parent", root());
        child_ = new DeleteMeWidget("child", parent_);
        WPushButton* delete_them = new WPushButton("Delete them both", root());
        delete_them->clicked().connect(this, &NotifyDeleteApp::delete_both);
    }

private:
    WContainerWidget* parent_;
    WContainerWidget* child_;

    void delete_both() {
        delete_server.emit(new DeleteEvent);
    }
};

WApplication* createNotifyDeleteApp(const WEnvironment& env) {
    return new NotifyDeleteApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createNotifyDeleteApp);
}

