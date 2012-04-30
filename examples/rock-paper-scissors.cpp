/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <map>
#include <set>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WDialog>
#include <Wt/WTable>

#include <Wt/Wc/Notify.hpp>
#include <Wt/Wc/GravatarImage.hpp>
#include <Wt/Wc/Planning.hpp>
#include <Wt/Wc/util.hpp>
#include <Wt/Wc/rand.hpp>

using namespace Wt;
using namespace Wt::Wc;
using namespace Wt::Wc::td;

notify::Server server;
notify::PlanningServer planning(&server);
boost::mutex key_to_score_mutex;

enum Choice {
    NOTHING, ROCK, PAPER, SCISSORS
};

std::string choice_to_text(Choice choice) {
    if (choice == NOTHING) {
        return "?";
    } else if (choice == ROCK) {
        return "Rock";
    } else if (choice == PAPER) {
        return "Paper";
    } else if (choice == SCISSORS) {
        return "Scissors";
    }
}

struct User : public notify::Event {
    std::string email;
    int score;

    User(const std::string& e):
        email(e), score(0)
    { }

    std::string key() const {
        return email;
    }
};

typedef boost::shared_ptr<User> UserPtr;

typedef std::map<std::string, UserPtr> Key2User;
Key2User key_to_user;

struct Game : public notify::Task {
    unsigned int id;
    UserPtr users[2];
    Choice choices[2];
    bool exceed_time_limit;
    boost::mutex mutex;

    Game(UserPtr user1, UserPtr user2):
        id(rr()), exceed_time_limit(false) {
        users[0] = user1;
        users[1] = user2;
        choices[0] = choices[1] = NOTHING;
    }

    std::string key() const {
        return "game-" + TO_S(id);
    }

    void process(notify::TaskPtr, notify::PlanningServer*) const {
        if (!winner()) {
            const_cast<Game*>(this)->exceed_time_limit = true;
        }
    }

    UserPtr winner() const {
        if (exceed_time_limit ||
                choices[0] == NOTHING || choices[1] == NOTHING ||
                choices[0] == choices[1]) {
            return UserPtr();
        } else if ((choices[0] == PAPER && choices[1] == ROCK) ||
                   (choices[0] == SCISSORS && choices[1] == PAPER) ||
                   (choices[0] == ROCK && choices[1] == SCISSORS)) {
            return users[0];
        } else {
            return users[1];
        }
    }

    int user_index(UserPtr user) const {
        return user == users[0] ? 0 : 1;
    }

    bool draw() const {
        return choices[0] != NOTHING && choices[0] == choices[1];
    }

    bool both() const {
        return  choices[0] != NOTHING &&  choices[1] != NOTHING;
    }

    GlobalColor user_color(UserPtr user) const {
        if (winner() == user) {
            return green;
        } else if (winner()) {
            return red;
        } else if (draw()) {
            return yellow;
        } else if (exceed_time_limit) {
            return magenta;
        }
    }

    bool can_change(UserPtr user) const {
        return choices[user_index(user)] == NOTHING && !exceed_time_limit;
    }

    void change(UserPtr user, Choice choice) {
        choices[user_index(user)] = choice;
        if (winner()) {
            winner()->score += 1;
            server.emit(winner());
        }
    }
};

typedef boost::shared_ptr<Game> GamePtr;

class UserImage : public GravatarImage {
public:
    UserImage(UserPtr user, WContainerWidget* parent = 0):
        GravatarImage(user->email, parent) {
        set_default(MONSTERID);
    }
};

void set_border_color(WWidget* widget, GlobalColor color) {
    WBorder border(WBorder::Double, WBorder::Thick, color);
    widget->decorationStyle().setBorder(border);
}

class GameWidget : public WContainerWidget, notify::Widget {
public:
    GameWidget(UserPtr me, GamePtr game, WContainerWidget* parent = 0):
        WContainerWidget(parent), notify::Widget(game->key(), &server),
        me_(me), game_(game) {
        notify(game);
    }

    void notify(notify::EventPtr /* event */) {
        boost::mutex::scoped_lock lock(game_->mutex);
        clear();
        for (int i = 0; i < 2; i++) {
            UserImage* image = new UserImage(game_->users[i], this);
            set_border_color(image, game_->user_color(game_->users[i]));
        }
        for (int i = 0; i < 2; i++) {
            Choice choice = game_->choices[i];
            if (me_ != game_->users[i] && !game_->both()) {
                choice = NOTHING;
            }
            WText* choice_text = new WText(choice_to_text(choice), this);
            set_border_color(choice_text, game_->user_color(game_->users[i]));
        }
        if (game_->can_change(me_)) {
            add_button(ROCK);
            add_button(PAPER);
            add_button(SCISSORS);
        }
    }

private:
    UserPtr me_;
    GamePtr game_;

    void add_button(Choice choice) {
        WPushButton* b = new WPushButton(choice_to_text(choice), this);
        b->clicked().connect(boost::bind(&GameWidget::select, this, choice));
    }

    void select(Choice choice) {
        boost::mutex::scoped_lock lock(game_->mutex);
        if (game_->can_change(me_)) {
            game_->change(me_, choice);
            server.emit(game_);
        }
    }
};

struct NewGame : public notify::Event {
    GamePtr game;
    UserPtr user;

    NewGame(GamePtr g, UserPtr u):
        game(g), user(u)
    { }

    std::string key() const {
        return "new-game-for-" + user->key();
    }
};

typedef boost::shared_ptr<NewGame> NewGamePtr;

class UserRecord : public WTable, public notify::Widget {
public:
    UserRecord(UserPtr user, UserPtr me, WContainerWidget* parent = 0):
        WTable(parent),
        notify::Widget(user->key(), &server),
        user_(user), me_(me) {
        UserImage* image = new UserImage(user_, elementAt(0, 0));
        if (user != me) {
            image->clicked().connect(this, &UserRecord::start_game_with);
            image->decorationStyle().setCursor(PointingHandCursor);
        }
        score_ = new WText(TO_S(user->score), elementAt(1, 0));
        elementAt(1, 0)->setContentAlignment(AlignCenter);
        setInline(true);
        set_border_color(this, gray);
    }

    void notify(notify::EventPtr /* event */) {
        boost::mutex::scoped_lock lock(key_to_score_mutex);
        if (key_to_user.find(user_->key()) == key_to_user.end()) {
            delete this;
        } else {
            score_->setText(TO_S(user_->score));
        }
    }

private:
    UserPtr user_;
    UserPtr me_;
    WText* score_;

    void start_game_with() {
        GamePtr game = boost::make_shared<Game>(user_, me_);
        planning.add(game, now() + 30 * SECOND);
        server.emit(boost::make_shared<NewGame>(game, user_));
        server.emit(boost::make_shared<NewGame>(game, me_));
    }
};

struct NewUser : public notify::Event {
    UserPtr user;

    NewUser(UserPtr u):
        user(u)
    { }

    std::string key() const {
        return "new-user";
    }
};

typedef boost::shared_ptr<NewUser> NewUserPtr;

class UserList : public WContainerWidget, public notify::Widget {
public:
    UserList(UserPtr me, WContainerWidget* parent = 0):
        WContainerWidget(parent),
        notify::Widget(NewUser(UserPtr()).key(), &server),
        me_(me) {
        boost::mutex::scoped_lock lock(key_to_score_mutex);
        BOOST_FOREACH (Key2User::value_type& key_and_user, key_to_user) {
            new UserRecord(key_and_user.second, me_, this);
        }
    }

    void notify(notify::EventPtr event) {
        const NewUser* e = DOWNCAST<const NewUser*>(event.get());
        if (e->user != me_) {
            addWidget(new UserRecord(e->user, me_));
        }
    }

private:
    UserPtr me_;
};

class RpsWidget : public WContainerWidget, public notify::Widget {
public:
    RpsWidget(UserPtr me, WContainerWidget* parent = 0):
        WContainerWidget(parent),
        notify::Widget(NewGame(GamePtr(), me).key(), &server),
        me_(me) {
        key_to_score_mutex.lock();
        key_to_user[me->key()] = me;
        key_to_score_mutex.unlock();
        addWidget(new UserRecord(me, me));
        addWidget(new WText("<hr />"));
        addWidget(new UserList(me));
        server.emit(boost::make_shared<NewUser>(me));
    }

    ~RpsWidget() {
        key_to_score_mutex.lock();
        key_to_user.erase(me_->key());
        key_to_score_mutex.unlock();
        for (Key2User::iterator it = key_to_user.begin();
                it != key_to_user.end(); ++it) {
        }
        server.emit(me_);
        BOOST_FOREACH (WDialog* dialog, std::set<WDialog*>(dialogs_)) {
            dialog->accept();
        }
    }

    void notify(notify::EventPtr event) {
        const NewGame* e = DOWNCAST<const NewGame*>(event.get());
        GamePtr game = e->game;
        WDialog* dialog = new WDialog("Game");
        dialog->contents()->addWidget(new GameWidget(me_, game));
        WPushButton* close = new WPushButton("Close");
        dialog->contents()->addWidget(close);
        close->clicked().connect(dialog, &WDialog::accept);
        dialog->finished().connect(this, &RpsWidget::delete_sender);
        dialog->setModal(false);
        dialog->show();
        dialogs_.insert(dialog);
    }

private:
    UserPtr me_;
    std::set<WDialog*> dialogs_;

    void delete_sender() {
        WDialog* dialog = DOWNCAST<WDialog*>(sender());
        dialogs_.erase(dialog);
        delete dialog;
    }
};

class RpsApp : public WApplication {
public:
    RpsApp(const WEnvironment& env):
        WApplication(env) {
        enableUpdates();
        do_logout();
    }

private:
    WLineEdit* email_;

    void do_logout() {
        root()->clear();
        email_ = new WLineEdit(rand_string(8) + "@test.com", root());
        WPushButton* enter = new WPushButton("Enter", root());
        enter->clicked().connect(this, &RpsApp::do_login);
    }

    void do_login() {
        if (key_to_user.find(email_->text().toUTF8()) != key_to_user.end()) {
            email_->setText("Already taken");
            return;
        }
        std::string email = email_->text().toUTF8();
        root()->clear();
        WPushButton* logout = new WPushButton("Log out", root());
        logout->clicked().connect(this, &RpsApp::do_logout);
        UserPtr me = boost::make_shared<User>(email);
        root()->addWidget(new RpsWidget(me));
    }
};

WApplication* createRockPaperScissorsApp(const WEnvironment& env) {
    return new RpsApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createRockPaperScissorsApp);
}

