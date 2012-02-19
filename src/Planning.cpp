/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <climits>
#include <vector>
#include <utility>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/tss.hpp>

#include "Planning.hpp"

namespace Wt {

namespace Wc {

namespace notify {

typedef std::pair<TaskPtr, WDateTime> TaskWhen;
typedef std::vector<TaskWhen> Tasks;
typedef boost::thread_specific_ptr<Tasks> TasksPtr;
TasksPtr tasks_ptr_;

Tasks& tasks() {
    if (tasks_ptr_.get() == 0) {
        tasks_ptr_.reset(new Tasks());
    }
    return *tasks_ptr_;
}

PlanningServer::PlanningServer(WIOService* io_service, WObject* p):
    WObject(p),
    server_(0),
    io_(io_service)
{ }

bool PlanningServer::add(TaskPtr task, const WDateTime& when,
                         bool immediately) {
    using namespace td;
    if (!when.isValid()) {
        return false;
    }
    if (immediately) {
        TimeDuration wait = when + delay_ - WDateTime::currentDateTime();
        schedule(wait, boost::bind(&PlanningServer::process, this, task));
    } else {
        tasks().push_back(std::make_pair(task, when));
    }
    return true;
}

bool PlanningServer::add(Task* task, WDateTime when, bool immediately) {
    return add(TaskPtr(task), when, immediately);
}

void PlanningServer::schedule(const td::TimeDuration& wait,
                              const boost::function<void()>& func) {
    int ms = wait.total_milliseconds();
    if (ms < 0) {
        ms = INT_MAX;
    }
    io_->schedule(ms, func);
}

void PlanningServer::process(TaskPtr task) {
    task->process(task, this);
    if (server_) {
        server_->emit(task);
    }
    if (tasks_ptr_.get()) {
        BOOST_FOREACH (TaskWhen task_when, tasks()) {
            add(task_when.first, task_when.second);
        }
        tasks().clear();
    }
}

}

}

}

