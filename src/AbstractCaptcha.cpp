/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <map>
#include "boost-xtime.hpp"
#include <boost/thread/mutex.hpp>

#include <Wt/WDateTime>
#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include "AbstractCaptcha.hpp"
#include "TimeDuration.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

AbstractCaptcha::AbstractCaptcha(WContainerWidget* parent):
    WCompositeWidget(parent),
    fault_(0),
    in_progress_(false),
    is_solved_(false)
{ }

AbstractCaptcha::~AbstractCaptcha() {
    delete fault_;
    fault_ = 0;
}

void AbstractCaptcha::check() {
    if (is_solved_) {
        solved_.emit();
    } else if (!in_progress_) {
        WString error = precheck_.empty() ? WString::Empty : precheck_();
        if (!error.empty()) {
            mistake(error);
        } else {
            in_progress_ = true;
            check_impl();
        }
    }
}

void AbstractCaptcha::update() {
    in_progress_ = false;
    is_solved_ = false;
    update_impl();
}

AbstractCaptcha::FaultSignal& AbstractCaptcha::fault() {
    if (!fault_) {
        fault_ = new FaultSignal(this);
    }
    return *fault_;
}

void AbstractCaptcha::set_buttons(bool)
{ }

void AbstractCaptcha::set_input(WFormWidget* /* input */)
{ }

namespace frequency_check_namespace {

boost::mutex mutex;
typedef std::map<std::string, WDateTime> Map;
Map ip2last;
int calls = 0;
const int FILTER_EVERY = 1000;
const td::TimeDuration INTERVAL = 3 * td::SECOND;

}

WString AbstractCaptcha::frequency_check() {
    using namespace frequency_check_namespace;
    boost::mutex::scoped_lock lock(mutex);
    WString result;
    const std::string ip = wApp->environment().clientAddress();
    Map::iterator it = ip2last.find(ip);
    if (it != ip2last.end()) {
        if (it->second + INTERVAL > now()) {
            result = tr("wc.captcha.Too_often");
        }
    }
    ip2last[ip] = now();
    calls += 1;
    if (calls >= FILTER_EVERY) {
        calls = 0;
        Map::iterator i = ip2last.begin();
        while (i != ip2last.end()) {
            if (i->second  + INTERVAL < now()) {
                ip2last.erase(i++);
            } else {
                ++i;
            }
        }
    }
    return result;
}

void AbstractCaptcha::solve() {
    in_progress_ = false;
    is_solved_ = true;
    solved_.emit();
}

void AbstractCaptcha::mistake(const WString& message) {
    update();
    if (fault_) {
        fault_->emit(message);
    }
}

}

}

