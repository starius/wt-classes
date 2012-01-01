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

Template parameter E is the type of event.

Create instance of class Server and bind it to WServer.
Inherit widgets from class Widget and implement notify() method.
Use Server::emit() method to notify all widgets, listenning
to this event.

Events are used as keys in std::map.
To provide additional information, not involved in comparison of events,
define comparison operator (operator<) for the event class
(to make different events look similar for std::map).
Furthermore, all events may be equal, in this case emit() method would
pass notification to all listening widgets.
*/

/** Base class for a widget to notify.

\ingroup notify
*/
template <typename E>
class Widget {
public:
    /** Constructor.
    \param e          Listening event
    \param server     Notification server
    \param app_id     Id of WApplication ("" means wApp->sessionId())
    */
    Widget(const E& e, Server<E>* server, const std::string& app_id = "");

    /** Destructor */
    virtual ~Widget();

    /** Notify.
    Implement this method for descendants: run updates caused by the event.
    */
    virtual void notify() = 0;

private:
    const E event_;
    Server<E>* server_;
    const std::string app_id_;
};

/** Notification server.
Notification server passes notifications to widgets.
This object is bound to server.

\ingroup notify
*/
template <typename E>
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
    void emit(const E& event);

private:
    typedef std::vector<Widget<E>*> Widgets;
    typedef std::map<std::string, Widgets> A2W;
    typedef std::map<E, A2W> O2W;
    O2W o2w_;
    boost::mutex mutex_;
    WServer* server_;

    void start_listenning(const E& event, Widget<E>* widget,
                          const std::string& app_id);

    void stop_listenning(const E& event, Widget<E>* widget,
                         const std::string& app_id);

    static void notify_widget(Widget<E>* widget);

    friend class Widget<E>;
};

}

}

}

#include "Notify_impl.hpp"

#endif

