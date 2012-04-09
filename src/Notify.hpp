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
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

#include <Wt/WGlobal>

#include "global.hpp"
#include "util.hpp"
#include "config.hpp"

namespace Wt {

namespace Wc {

/** Namespace for notifications, passed to widgets

\ingroup notify
*/
namespace notify {

/** \defgroup notify Notification server
Notifications, passed to widgets.

This compound is similar to signal/slot system, but:
 - widgets are listening to the particular event, not to all possible events,
   (like slots in signal/slot system do);
 - notification server is shared across sessions.

Each event has appropriate key.
When event is thrown, the only widgets with this key are notified.

Create instance of class Server and bind it to WServer.
Inherit widgets from class Widget and implement notify() method.
Use Server::emit() method to notify all widgets, listening
to this event.

To execute some actions scheduling, use PlanningServer class.
It can work in conjunction with a notification server.
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

    /** Convert to Key type */
    operator Key() const;
};

/** Shared pointer to an event.

\ingroup notify
*/
typedef boost::shared_ptr<const Event> EventPtr ;

/** Base class for a widget to notify.

\ingroup notify
*/
class Widget {
public:
    /** Constructor.
    \param key        Event key to listen
    \param server     Notification server
    \param app_id     Deprecated, not used
    When created, wApp must return current WApplication.
    */
    Widget(const Event::Key& key, Server* server,
           const std::string& app_id = "");

    /** Destructor */
    virtual ~Widget();

    /** Notify.
    Implement this method for descendants: run updates caused by the event.
    */
    virtual void notify(EventPtr event) = 0;

    /** Get event key */
    const Event::Key& key() const {
        return key_;
    }

private:
    const Event::Key key_;
    Server* server_;
    WApplication* app_id_;
};

/** Notification server.
Notification server passes notifications to widgets.
This object is bound to server.

\ingroup notify
*/
class Server {
public:
    /** Constructor.
    \param server Deprecated, not used
    */
    Server(WServer* server = 0);

    /** Notify all widgets, listening to object updates.
    After all widgets of an application were notified,
    updates_trigger() is called.
    \attention If you use transactions, call this method
               after successful transaction committing.
    */
    void emit(EventPtr event);

    /** Notify all widgets, listening to object updates.
    This is an overloaded method for convenience.
    Ownership of the event is transferred.
    */
    void emit(Event* event);

private:
    typedef std::vector<Widget*> Widgets;
    typedef std::map<WApplication*, Widgets> A2W;
    typedef std::map<Event::Key, A2W> O2W;
    typedef std::map<WApplication*, OneAnyFunc> A2F;
    O2W o2w_;
    A2F a2f_;
    boost::mutex mutex_;

    void start_listening(Widget* widget);

    void stop_listening(Widget* widget, WApplication* app_id);

    void notify_widgets(const boost::any& event);

    friend class Widget;
};

}

}

}

#endif

