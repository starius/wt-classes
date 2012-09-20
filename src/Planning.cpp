/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "config.hpp"

#include <vector>
#include <utility>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/tss.hpp>

#include "Planning.hpp"
#include "util.hpp"
#include "config.hpp"

#define USE_WIOSERVICE (defined(WC_HAVE_WIOSERVICE) && \
        defined(WC_HAVE_ENVIRONMENT_SERVER))

#if USE_WIOSERVICE
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WServer>
#endif

namespace Wt {

namespace Wc {

namespace notify {

typedef std::pair<TaskPtr, WDateTime> TaskWhen;
typedef std::vector<TaskWhen> Tasks;

struct ThreadState {
    ThreadState():
        is_processing(false)
    { }

    bool is_processing;
    Tasks queue;
};

typedef boost::thread_specific_ptr<ThreadState> ThreadStatePtr;
ThreadStatePtr state_ptr_;

static ThreadState& state() {
    if (state_ptr_.get() == 0) {
        state_ptr_.reset(new ThreadState());
    }
    return *state_ptr_;
}

PlanningServer::PlanningServer(WIOService* /* io_service */, WObject* p):
    WObject(p),
    server_(0)
{ }

PlanningServer::PlanningServer(WObject* p):
    WObject(p),
    server_(0)
{ }

PlanningServer::PlanningServer(Server* notification_server, WObject* p):
    WObject(p),
    server_(notification_server)
{ }

bool PlanningServer::add(TaskPtr task, const WDateTime& when) {
    using namespace td;
    if (!when.isValid()) {
        return false;
    }
    if (!state().is_processing) {
        TimeDuration wait = when + delay_ - WDateTime::currentDateTime();
        schedule(wait, boost::bind(&PlanningServer::process, this, task));
    } else {
        state().queue.push_back(std::make_pair(task, when));
    }
    return true;
}

bool PlanningServer::add(Task* task, WDateTime when) {
    return add(TaskPtr(task), when);
}

bool PlanningServer::add(TaskPtr task, const WDateTime& when,
                         bool /* immediately */) {
    return add(task, when);
}

bool PlanningServer::add(Task* task, WDateTime when, bool /* immediately */) {
    return add(TaskPtr(task), when);
}

WIOService* PlanningServer::io_service() {
#if USE_WIOSERVICE
    return &WServer::instance()->ioService();
#else
    return 0;
#endif
}

void PlanningServer::schedule(const td::TimeDuration& wait,
                              const boost::function<void()>& func) {
    schedule_action(wait, func);
}

void PlanningServer::process(TaskPtr task) {
    state().is_processing = true;
    task->process(task, this);
    state().is_processing = false;
    if (server_) {
        server_->emit(task);
    }
    if (!state().queue.empty()) {
        BOOST_FOREACH (TaskWhen task_when, state().queue) {
            add(task_when.first, task_when.second);
        }
        state().queue.clear();
    }
}

}

}

}

