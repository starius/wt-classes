/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_PLANNING_SERVER_HPP_
#define WC_PLANNING_SERVER_HPP_

#include <Wt/WObject>
#include <Wt/WDateTime>
#include <Wt/WIOService>

#include "Notify.hpp"
#include "TimeDuration.hpp"

namespace Wt {

namespace Wc {

namespace notify {

/** Shared pointer to a task.

\ingroup notify
*/
typedef boost::shared_ptr<const Task> TaskPtr ;
/** Task abstract class.

\ingroup notify
*/
class Task : public Event {
public:
    /** Task processing method.
    This method could use \c planning_server to add itself or other tasks.
    \attention This method must not throw exceptions!
    */
    virtual void process(TaskPtr task, PlanningServer* server) const = 0;
};

/** Planning server.

\ingroup notify
*/
class PlanningServer : public WObject {
public:
    /** Constructor */
    PlanningServer(WIOService* io_service, WObject* p = 0);

    /** Add a task to the planning list.
    If the \c when  is \c inValid() (e.g., Null), no action is performed
    (in this case \c false is returned, otherwise \c true).

    The task is executed at <tt>when + delay()</tt>.

    If immediately is false, the task would be added after currently running
    task is finished and notification server is emitted with current event.
    Using immediately = false makes sense only from Task::process().
    This can be useful if new added task depends on
    uncommitted results of current task.
    */
    bool add(TaskPtr task, const WDateTime& when, bool immediately = true);

    /** Add a task to the planning list.
    This is an overloaded method for convenience.
    Ownership of the task is transferred.
    */
    bool add(Task* task, WDateTime when, bool immediately = true);

    /** Get delay.
    \see add()
    */
    const td::TimeDuration& delay() const {
        return delay_;
    }

    /** Set delay.
    \see add()
    \note This does not affect already added tasks.
    */
    void set_delay(const td::TimeDuration& value) {
        delay_ = value;
    }

    /** Set notification server (optional).
    The server is emitted after processing of a task.

    \note The ownership of the notification server is not transferred.
    */
    void set_notification_server(Server* server) {
        server_ = server;
    }

    /** Get notification server */
    Server* notification_server() {
        return server_;
    }

    /** Get IO service */
    WIOService* io_service() {
        return io_;
    }

    /** Set IO service */
    void set_io_service(WIOService* io_service) {
        io_ = io_service;
    }

    /** Utility method used to schedule a function.
    \note The function would be executed in "raw" thread of io service.
        Consider using of bound_post() wrapper for \p func.
    */
    void schedule(const td::TimeDuration& wait,
                  const boost::function<void()>& func);

private:
    Server* server_;
    td::TimeDuration delay_;
    WIOService* io_;

    void process(TaskPtr task);
};

}

}

}

#endif

