/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WDateTime>

#include "TimeDuration.hpp"
#include "rand.hpp"

namespace Wt {

namespace Wc {

namespace td {

namespace ptime = boost::posix_time;

std::string td2str(const TimeDuration& td) {
    return ptime::to_simple_string(ptime::seconds(td.total_seconds()));
}

TimeDuration operator -(const Wt::WDateTime& a, const Wt::WDateTime& b) {
    return a.toPosixTime() - b.toPosixTime();
}

Wt::WDateTime operator +(const Wt::WDateTime& a, const TimeDuration& b) {
    return Wt::WDateTime::fromPosixTime(a.toPosixTime() + b);
}

Wt::WDateTime operator -(const Wt::WDateTime& a, const TimeDuration& b) {
    return a + (-b);
}

Wt::WDateTime& operator +=(Wt::WDateTime& a, const TimeDuration& b) {
    return a = a + b;
}

Wt::WDateTime& operator -=(Wt::WDateTime& a, const TimeDuration& b) {
    return a = a - b;
}

TimeDuration operator /(const TimeDuration& a, const double& b) {
    return ptime::milliseconds(double(a.total_milliseconds()) /  b);
}

TimeDuration operator *(const TimeDuration& a, const double& b) {
    return ptime::milliseconds(double(a.total_milliseconds()) *  b);
}

TimeDuration operator *(const double& b, const TimeDuration& a) {
    return a * b;
}

double operator /(const TimeDuration& a, const TimeDuration& b) {
    return double(a.total_milliseconds()) / double(b.total_milliseconds());
}

Wt::WDateTime now() {
    return Wt::WDateTime::currentDateTime();
}

long total_minutes(const TimeDuration& t) {
    return t.total_seconds() / 60;
}

TimeDuration rand_range(const TimeDuration& start, const TimeDuration& stop) {
    unsigned int start_int = start.total_milliseconds();
    unsigned int stop_int = stop.total_milliseconds();
    return ptime::milliseconds(rr(start_int, stop_int));
}

}

}

}

