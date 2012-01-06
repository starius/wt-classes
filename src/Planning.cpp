/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include "Planning.hpp"

namespace Wt {

namespace Wc {

namespace notify {

PlanningServer::PlanningServer():
    WIOService(),
    server_(0)
{ }

bool PlanningServer::add(TaskPtr task, const WDateTime& when) {
    using namespace td;
    if (!when.isValid()) {
        return false;
    }
    TimeDuration wait = when + delay_ - WDateTime::currentDateTime();
    int ms = wait.total_milliseconds();
    schedule(ms, boost::bind(&PlanningServer::process, this, task));
    return true;
}

bool PlanningServer::add(Task* task, WDateTime when) {
    return add(TaskPtr(task), when);
}

void PlanningServer::process(TaskPtr task) {
    task->process(task, this);
    if (server_) {
        server_->emit(task);
    }
}

}

}

}

