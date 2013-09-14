/*
 * wnoref, utility classes used by Wt applications
 * Copyright (C) 2013 Boris Nagaev
 *
 * wnoref is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 2 of the License.
 *
 * wnoref is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License v2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with wnoref.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <map>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WTextArea>
#include <Wt/WLineEdit>
#include <Wt/WAnchor>
#include <Wt/WPushButton>

#include <Wt/Wc/Planning.hpp>
#include <Wt/Wc/util.hpp>
#include <Wt/Wc/Url.hpp>
#include <Wt/Wc/TimeDuration.hpp>
#include <Wt/Wc/ConstrainedSpinBox.hpp>
#include <Wt/Wc/rand.hpp>

using namespace Wt;
using namespace Wt::Wc;
using namespace Wt::Wc::url;

class Note;
typedef boost::shared_ptr<Note> NotePtr;
typedef std::map<std::string, NotePtr> Key2Note;

Key2Note key_to_note_;
boost::mutex key_to_note_mutex_;
notify::PlanningServer planning_;

struct Note : public notify::Task {
    std::string key_;
    WString text_;

    std::string key() const {
        return "note-" + key_;
    }

    void process(notify::TaskPtr, notify::PlanningServer*) const {
        // removes itself
        boost::mutex::scoped_lock lock(key_to_note_mutex_);
        key_to_note_.erase(key_);
    }
};

class WnorefApp : public WApplication {
public:
    WnorefApp(const WEnvironment& env):
        WApplication(env) {
        root()->clear();
        root()->addWidget(new WText("Enter text to be shown only once"));
        root()->addWidget(new WBreak);
        root()->addWidget(new WText("Text is stored in the RAM, "
                                    "not on hard drive"));
        root()->addWidget(new WBreak);
        textarea_ = new WTextArea(root());
        textarea_->setRows(10);
        textarea_->setColumns(80);
        root()->addWidget(new WBreak);
        WPushButton* get_link = new WPushButton("Get link!", root());
        get_link->clicked().connect(this, &WnorefApp::do_get_link);
        root()->addWidget(new WText("Store max "));
        delay_ = new ConstrainedSpinBox(root());
        delay_->setRange(1, 60);
        delay_->setValue(5);
        root()->addWidget(new WText(" minutes"));
        root()->addWidget(new WBreak);
        url_edit_ = new WLineEdit(root());
        url_edit_->setTextSize(50);
        url_edit_->hide();
        parser_ = new Parser(this);
        note_url_ = new StringNode(parser_);
        parser_->connect(parser_,
                         boost::bind(&WnorefApp::show_main, this));
        parser_->connect(note_url_,
                         boost::bind(&WnorefApp::show_note, this));
        parser_->open(internalPath());
        internalPathChanged().connect(parser_, &Parser::open);
    }

    void show_main() {
        textarea_->setText("");
        url_edit_->setText("");
    }

    void do_get_link() {
        NotePtr note(new Note);
        std::string key = rand_string(12);
        note->key_ = key;
        note->text_ = textarea_->text();
        {
            boost::mutex::scoped_lock lock(key_to_note_mutex_);
            key_to_note_[key] = note;
        }
        int delay_mins = delay_->corrected_value();
        planning_.add(note, now() + delay_mins * td::MINUTE);
        //
        note_url_->set_string(key);
        std::string url = environment().hostName() +
                          environment().deploymentPath() +
                          note_url_->full_path();
        boost::algorithm::replace_all(url, "//", "/");
        url = url_scheme() + "://" + url;
        url_edit_->setText(url);
        doJavaScript(url_edit_->jsRef() + ".select();");
        doJavaScript(url_edit_->jsRef() + ".focus();");
        url_edit_->show();
    }

    void show_note() {
        boost::mutex::scoped_lock lock(key_to_note_mutex_);
        std::string key = note_url_->value();
        Key2Note::iterator it = key_to_note_.find(key);
        if (it == key_to_note_.end()) {
            textarea_->setText("No note with this key!");
        } else {
            NotePtr note = it->second;
            textarea_->setText(note->text_);
            key_to_note_.erase(it); // show text only one time
        }
        url_edit_->hide();
    }

private:
    Parser* parser_;
    StringNode* note_url_;
    WTextArea* textarea_;
    ConstrainedSpinBox* delay_;
    WLineEdit* url_edit_;
};

WApplication* createWnorefApp(const WEnvironment& env) {
    return new WnorefApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createWnorefApp);
}

