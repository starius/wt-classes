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
typedef boost::shared_ptr<const Event> EventPtr;

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

    /** Return if this widget needs page updates.
    This method is called as a result of Server::emit(),
    before notify() method.
    If at least one Widget of WApplication returned \c true
    from updates_needed(), AND Server::updates_enabled(),
    updates_trigger() is called.

    Defaults to \c true.

    \note This is only possible after a call to wApp->enableUpdates()
    */
    virtual bool updates_needed(EventPtr event) const {
        return true;
    }

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
    updates_trigger() is called,
    if at least one Widget of WApplication returned \c true
    from Widget::updates_needed(), AND Server::updates_enabled(),
    updates_trigger() is called.
    \attention If you use transactions, call this method
               after successful transaction committing.

    \note This is only possible after a call to wApp->enableUpdates()
    */
    void emit(EventPtr event) const;

    /** Notify all widgets, listening to object updates.
    This is an overloaded method for convenience.
    Ownership of the event is transferred.
    */
    void emit(Event* event) const;

    /** Get if the server can call updates_trigger() */
    bool updates_enabled() const {
        return updates_enabled_;
    }

    /** Set if the server can call updates_trigger().
    Defaults to \c true.

    \note This is only possible after a call to wApp->enableUpdates()
    */
    void set_updates_enabled(bool updates_enabled) {
        updates_enabled_ = updates_enabled;
    }

    /** Get if current application is notified directly (no bound_post) */
    bool direct_to_this() const {
        return direct_to_this_;
    }

    /** Set if current application is notified directly (no bound_post).
    Defaults to \c false.

    \note Setting this to \c true, you fix delay of events in HTML version
        in case of self-notification.
        However you should take this behavior into account
        (emitting events may cause dead lock, being done under lock),
        so this option is switched off by default.
    */
    void set_direct_to_this(bool direct_to_this) {
        direct_to_this_ = direct_to_this;
    }

private:
    typedef boost::shared_ptr<OneAnyFunc> Poster;
    typedef std::vector<Widget*> Widgets;
    typedef std::pair<Poster, Widgets> PosterAndWidgets;
    typedef std::map<WApplication*, PosterAndWidgets> A2W;
    typedef std::map<Event::Key, A2W> O2W;
    O2W o2w_;
    mutable boost::mutex mutex_;
    bool updates_enabled_;
    bool direct_to_this_;

    void start_listening(Widget* widget);

    void stop_listening(Widget* widget, WApplication* app_id);

    void notify_widgets(const boost::any& event) const;

    friend class Widget;
};

}

}

}

#endif

