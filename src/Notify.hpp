/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_NOTIFY_HPP_
#define WC_NOTIFY_HPP_

#include <map>
#include <boost/thread/mutex.hpp>

#include <Wt/WGlobal>

#include "global.hpp"

namespace Wt {

namespace Wc {

namespace notify {

/** \defgroup notify Notification server
Notifications, passed to widgets.

This compound is similar to signal/slot system, but:
 - widgets are listening to the particular event, not to all possible events,
   like slots in signal/slot system;
 - notification server is shared across sessions.

Each event has appropriate key.
When event is thrown, the only widgets with this key are notified.

Create instance of class Server and bind it to WServer.
Inherit widgets from class Widget and implement notify() method.
Use Server::emit() method to notify all widgets, listenning
to this event.
*/

/** Event abstract class.

\ingroup notify
*/
class Event {
public:
    /** Type of key used to select widgets to notify */
    typedef std::string Key;

    /** Get key */
    virtual Key key() const = 0;
};

/** Base class for a widget to notify.

\ingroup notify
*/
class Widget {
public:
    /** Constructor.
    \param key        Event key to listen
    \param server     Notification server
    \param app_id     Id of WApplication ("" means wApp->sessionId())
    */
    Widget(const Event::Key& key, Server* server,
           const std::string& app_id = "");

    /** Destructor */
    virtual ~Widget();

    /** Notify.
    Implement this method for descendants: run updates caused by the event.
    */
    virtual void notify() = 0;

private:
    const Event::Key key_;
    Server* server_;
    const std::string app_id_;
};

/** Notification server.
Notification server passes notifications to widgets.
This object is bound to server.

\ingroup notify
*/
class Server {
public:
    /** Constructor.
    \param server WServer to run post() method (0 means WServer::instance())
    */
    Server(WServer* server = 0);

    /** Notify all widgets, listening to object updates.
    After all widgets of an application were notified,
    triggerUpdate() is posted.
    \attention If you use transactions, call this method
               after successful transaction committing.
    */
    void emit(Event* event);

private:
    typedef std::vector<Widget*> Widgets;
    typedef std::map<std::string, Widgets> A2W;
    typedef std::map<Event::Key, A2W> O2W;
    O2W o2w_;
    boost::mutex mutex_;
    WServer* server_;

    void start_listenning(const Event::Key& key, Widget* widget,
                          const std::string& app_id);

    void stop_listenning(const Event::Key& key, Widget* widget,
                         const std::string& app_id);

    static void notify_widget(Widget* widget);

    friend class Widget;
};

}

}

}

#endif

