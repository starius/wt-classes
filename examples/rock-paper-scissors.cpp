/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <map>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

#include <Wt/WApplication>
#include <Wt/WEnvironment>
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

enum Choice {
    NOTHING, ROCK = 1, PAPER = 2, SCISSORS = 3
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
boost::mutex key_to_user_mutex;

// Null in result means that name already taken
// On logout you must remove record from key_to_user!
UserPtr create_user(const std::string& email) {
    boost::mutex::scoped_lock lock(key_to_user_mutex);
    if (key_to_user.find(email) != key_to_user.end()) {
        return UserPtr();
    } else {
        UserPtr result = boost::make_shared<User>(email);
        key_to_user[result->key()] = result;
        return result;
    }
}

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
        return choices[0] != NOTHING &&  choices[1] != NOTHING;
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
        return white;
    }

    GlobalColor user_color(int i) const {
        return user_color(users[i]);
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

class GameWidget : public WTable, notify::Widget {
public:
    GameWidget(UserPtr me, GamePtr game, WContainerWidget* parent = 0):
        WTable(parent), notify::Widget(game->key(), &server),
        me_(me), game_(game) {
        for (int i = 0; i < 2; i++) {
            UserImage* image = new UserImage(game_->users[i], elementAt(0, i));
            WText* choice_text = new WText(elementAt(1, i));
            elementAt(1, i)->setContentAlignment(AlignCenter);
        }
        WTableCell* choice_cell = elementAt(2, 0);
        choice_cell->setColumnSpan(2);
        add_button(ROCK, choice_cell);
        add_button(PAPER, choice_cell);
        add_button(SCISSORS, choice_cell);
        notify(game);
        removed_.reset(new bool);
        *removed_ = false;
    }

    ~GameWidget() {
        *removed_ = true;
    }

    void notify(notify::EventPtr /* event */) {
        boost::mutex::scoped_lock lock(game_->mutex);
        for (int i = 0; i < 2; i++) {
            set_border_color(elementAt(0, i)->widget(0), game_->user_color(i));
            Choice choice = game_->choices[i];
            if (me_ != game_->users[i] && !game_->both()) {
                choice = NOTHING;
            }
            WText* choice_text = DOWNCAST<WText*>(elementAt(1, i)->widget(0));
            choice_text->setText(choice_to_text(choice));
        }
        if (!game_->can_change(me_)) {
            WTableCell* choice_cell = elementAt(2, 0);
            choice_cell->clear();
        }
    }

    boost::shared_ptr<bool> removed_;

private:
    UserPtr me_;
    GamePtr game_;

    void add_button(Choice choice, WContainerWidget* parent) {
        WPushButton* b = new WPushButton(choice_to_text(choice), parent);
        b->clicked().connect(boost::bind(&GameWidget::select, this, choice));
    }

    void select(Choice choice) {
        game_->mutex.lock();
        bool emit = false;
        if (game_->can_change(me_)) {
            game_->change(me_, choice);
            emit = true;
        }
        game_->mutex.unlock();
        if (emit) {
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

const td::TimeDuration GAME_TIME_LIMIT = 30 * SECOND;

class UserRecord : public WTable, public notify::Widget {
public:
    UserRecord(UserPtr user, UserPtr me, WContainerWidget* parent = 0):
        WTable(parent),
        notify::Widget(user->key(), &server),
        user_(user), me_(me) {
        UserImage* image = new UserImage(user_, elementAt(0, 0));
        score_ = new WText(TO_S(user->score), elementAt(1, 0));
        if (user != me) {
            image->clicked().connect(this, &UserRecord::start_game_with);
            image->decorationStyle().setCursor(PointingHandCursor);
            score_->clicked().connect(this, &UserRecord::start_game_with);
            score_->decorationStyle().setCursor(PointingHandCursor);
        }
        elementAt(1, 0)->setContentAlignment(AlignCenter);
        setInline(true);
        set_border_color(this, gray);
    }

    void notify(notify::EventPtr /* event */) {
        boost::mutex::scoped_lock lock(key_to_user_mutex);
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
        planning.add(game, now() + GAME_TIME_LIMIT);
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
        boost::mutex::scoped_lock lock(key_to_user_mutex);
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

void close_dialog(WDialog* dialog, boost::shared_ptr<bool> removed) {
    if (!*removed) {
        dialog->reject();
        updates_trigger();
    }
}

class RpsWidget : public WContainerWidget, public notify::Widget {
public:
    RpsWidget(UserPtr me, WContainerWidget* parent = 0):
        WContainerWidget(parent),
        notify::Widget(NewGame(GamePtr(), me).key(), &server),
        me_(me) {
        addWidget(new UserRecord(me, me));
        addWidget(new WText("<hr />"));
        addWidget(new WText("Click on the player you want to play with"));
        addWidget(new UserList(me));
        server.emit(boost::make_shared<NewUser>(me));
    }

    ~RpsWidget() {
        key_to_user_mutex.lock();
        key_to_user.erase(me_->key());
        key_to_user_mutex.unlock();
        server.emit(me_);
        removed_.emit();
    }

    void notify(notify::EventPtr event) {
        const NewGame* e = DOWNCAST<const NewGame*>(event.get());
        GamePtr game = e->game;
        WDialog* dialog = new WDialog("Game");
        GameWidget* game_widget = new GameWidget(me_, game);
        dialog->contents()->addWidget(game_widget);
        set_closable(dialog);
        delete_closed(dialog);
        dialog->setModal(false);
        dialog->show();
        removed_.connect(dialog, &WDialog::reject);
        schedule_action(2 * GAME_TIME_LIMIT,
                        bound_post(boost::bind(close_dialog,
                                               dialog, game_widget->removed_)));
    }

private:
    UserPtr me_;
    Signal<> removed_;
};

std::string random_email() {
    return rand_string(8) + "@test.com";
}

class BotWatcher : public WObject, public notify::Widget {
public:
    BotWatcher(UserPtr me, WObject* parent = 0):
        WObject(parent),
        notify::Widget(NewGame(GamePtr(), me).key(), &server),
        me_(me) {
        server.emit(boost::make_shared<NewUser>(me));
    }

    ~BotWatcher() {
        key_to_user_mutex.lock();
        key_to_user.erase(me_->key());
        key_to_user_mutex.unlock();
        server.emit(me_);
    }

    void notify(notify::EventPtr event) {
        const NewGame* e = DOWNCAST<const NewGame*>(event.get());
        GamePtr game = e->game;
        schedule_action(rand_range(2 * td::SECOND, 5 * td::SECOND),
                        boost::bind(&BotWatcher::choose, this, game));
    }

    void choose(GamePtr game) {
        bool emit = false;
        game->mutex.lock();
        if (game->can_change(me_)) {
            Choice choice = Choice(rr(1, 4));
            game->change(me_, choice);
            emit = true;
        }
        game->mutex.unlock();
        if (emit) {
            server.emit(game);
        }
    }

private:
    UserPtr me_;
};

const int BOT_MINPLAYERS = 5;

void update_bots() {
    key_to_user_mutex.lock();
    int players = key_to_user.size();
    key_to_user_mutex.unlock();
    if (players < BOT_MINPLAYERS) {
        UserPtr bot = create_user(random_email());
        if (bot) {
            new BotWatcher(bot);
        }
    }
}

class RpsApp : public WApplication {
public:
    RpsApp(const WEnvironment& env):
        WApplication(env) {
        enableUpdates();
        do_logout();
        schedule_action(td::TD_NULL, update_bots);
    }

private:
    WLineEdit* email_;

    void do_logout() {
        root()->clear();
        email_ = new WLineEdit(random_email(), root());
        WPushButton* enter = new WPushButton("Enter", root());
        enter->clicked().connect(this, &RpsApp::do_login);
    }

    void do_login() {
        std::string email = email_->text().toUTF8();
        UserPtr me = create_user(email);
        if (!me) {
            email_->setText("Already taken");
            return;
        }
        root()->clear();
        WPushButton* logout = new WPushButton("Log out", root());
        logout->clicked().connect(this, &RpsApp::do_logout);
        if (!wApp->environment().ajax()) {
            WPushButton* update = new WPushButton("Update", root());
            update->clicked().connect(update, &WWidget::show); // do nothing
        }
        root()->addWidget(new RpsWidget(me));
    }
};

struct ServerConfig {
    ServerConfig() {
        server.set_direct_to_this(true);
    }
} server_config;

WApplication* createRockPaperScissorsApp(const WEnvironment& env) {
    return new RpsApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createRockPaperScissorsApp);
}

