/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WPushButton>

#include <Wt/Wc/Notify.hpp>
#include <Wt/Wc/util.hpp>

using namespace Wt;
using namespace Wt::Wc;

notify::Server notify_server;

class NumberText : public WText, public notify::Widget {
public:
    NumberText( WContainerWidget* parent):
        WText(parent),
        notify::Widget(&notify_server),
        number_(0) {
        setText(TO_S(number_));
    }

protected:
    void notify(notify::EventPtr /* event */) {
        number_ += 1;
        setText(TO_S(number_));
    }

private:
    int number_;
};

class NumberContainer : public WContainerWidget {
public:
    NumberContainer() {
        text_ = new NumberText(this);
        key_edit_ = new WLineEdit(this);
        key_edit_->setEmptyText("key");
        WPushButton* add = new WPushButton("Add", this);
        add->clicked().connect(this, &NumberContainer::add_key);
        WPushButton* remove = new WPushButton("Remove", this);
        remove->clicked().connect(this, &NumberContainer::remove_key);
        keys_ = new WContainerWidget(this);
        keys_->setInline(true);
    }

private:
    NumberText* text_;
    WLineEdit* key_edit_;
    WContainerWidget* keys_;

    void add_key() {
        text_->start_listening(key_edit_->text().toUTF8());
        update_keys();
    }

    void remove_key() {
        text_->stop_listening(key_edit_->text().toUTF8());
        update_keys();
    }

    void update_keys() {
        keys_->clear();
        BOOST_FOREACH (std::string key, text_->keylist()) {
            keys_->addWidget(new Wt::WText(" " + key));
        }
    }
};

class NotifyApp : public WApplication {
public:
    NotifyApp(const WEnvironment& env):
        WApplication(env) {
        enableUpdates();
        for (int i = 0; i < 5; i++) {
            root()->addWidget(new NumberContainer);
        }
        key_ = new WLineEdit(root());
        key_->setEmptyText("key");
        WPushButton* emit = new WPushButton("Emit", root());
        emit->clicked().connect(this, &NotifyApp::emit_key);
    }

private:
    WLineEdit* key_;

    void emit_key() {
        notify_server.emit(key_->text().toUTF8());
    }
};

WApplication* createNotifyApp(const WEnvironment& env) {
    return new NotifyApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createNotifyApp);
}

