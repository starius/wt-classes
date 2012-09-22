/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_PLANNING_SERVER_HPP_
#define WC_PLANNING_SERVER_HPP_

#include <boost/function.hpp>

#include <Wt/WObject>
#include <Wt/WDateTime>

#include "Notify.hpp"
#include "TimeDuration.hpp"

namespace Wt {

class WIOService; // FIXME http://redmine.emweb.be/issues/1189

namespace Wc {

namespace notify {

/** Shared pointer to a task.

\ingroup notify
*/
typedef boost::shared_ptr<const Task> TaskPtr;

/** Task abstract class.

\ingroup notify
*/
class Task : public Event {
public:
    /** Task processing method.
    This method could use \c server to add itself or other tasks.
    \attention This method must not throw exceptions!
    */
    virtual void process(TaskPtr task, PlanningServer* server) const = 0;

    /** Return if this should be passed to notify::Server after process() */
    bool notify_needed() const {
        return notify_needed_;
    }

    /** Set if this should be passed to notify::Server after process().
    Prior to run Task::process() the value of this is set to
    PlanningServer::default_notify_needed()
    by PlanningServer.
    So you should call Task::set_notify_needed() only from process().

    This method can be used to manage notification per-task.
    */
    void set_notify_needed(bool notify_needed) const {
        notify_needed_ = notify_needed;
    }

private:
    mutable bool notify_needed_;
};

/** Planning server.

\ingroup notify
*/
class PlanningServer : public WObject {
public:
    /** Constructor.
    \deprecated io_service is ignored. Use another constructor
    */
    PlanningServer(WIOService* io_service, WObject* p = 0);

    /** Constructor */
    PlanningServer(WObject* p = 0);

    /** Constructor.
    \see set_notification_server()
    */
    PlanningServer(Server* notification_server, WObject* p = 0);

    /** Add a task to the planning list.
    If the \c when is \c inValid() (e.g., Null), no action is performed
    (in this case \c false is returned, otherwise \c true).

    The task is executed at <tt>when + delay()</tt>.

    If this is called from code processing a task, then new task will be added
    to internal thread local queue, which will be added afterwards.

    \see schedule
    */
    bool add(TaskPtr task, const WDateTime& when);

    /** Add a task to the planning list.
    This is an overloaded method for convenience.
    Ownership of the task is transferred.
    */
    bool add(Task* task, WDateTime when);

    /** Add a task to the planning list.
    \deprecated
    */
    bool add(TaskPtr task, const WDateTime& when, bool immediately);

    /** Add a task to the planning list.
    \deprecated
    */
    bool add(Task* task, WDateTime when, bool immediately);

    /** Get delay.
    \see add()
    */
    const td::TimeDuration& delay() const {
        return delay_;
    }

    /** Set delay.
    \see add()
    Defaults to zero delay.
    \note This does not affect already added tasks.
    */
    void set_delay(const td::TimeDuration& value) {
        delay_ = value;
    }

    /** Set notification server (optional).
    The server is emitted after processing of a task, if Task::notify_needed().

    \note The ownership of the notification server is not transferred.

    \see set_default_notify_needed()
    */
    void set_notification_server(Server* server) {
        server_ = server;
    }

    /** Get notification server */
    Server* notification_server() {
        return server_;
    }

    /** Return the value of Task::notify_needed() set before Task::process() */
    bool default_notify_needed() const {
        return default_notify_needed_;
    }

    /** Set the value of Task::notify_needed() set before Task::process().
    Defaults to true.

    This method can be used to manage notification globally.

    \see Task::set_notify_needed()
    */
    void set_default_notify_needed(bool default_notify_needed) {
        default_notify_needed_ = default_notify_needed;
    }

    /** Get IO service.
    \deprecated Return WIOService, used for Wt server, if available, else 0.
    */
    WIOService* io_service();

    /** Set IO service.
    \deprecated Does nothing
    */
    void set_io_service(WIOService*)
    { }

    /** Utility method used to schedule a function.
    This is a method for convenience.
    \see schedule_action()
    */
    void schedule(const td::TimeDuration& wait,
                  const boost::function<void()>& func);

private:
    Server* server_;
    td::TimeDuration delay_;
    bool default_notify_needed_;

    void process(TaskPtr task);
};

}

}

}

#endif

