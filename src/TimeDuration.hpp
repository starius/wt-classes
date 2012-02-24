/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_TIME_DURATION_HPP_
#define WC_TIME_DURATION_HPP_

#include <string>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <Wt/WGlobal>

namespace Wt {

namespace Wc {

/** \defgroup time Time-related
Classes and typedef simplifying time management.
*/

/** Namespace for time duration constants and integration with WDateTime.
\ingroup time
*/
namespace td {

/** Time duration */
class TimeDuration : public boost::posix_time::time_duration {
public:
    /** Base class */
    typedef boost::posix_time::time_duration Base;

    /** Default constructor (0 seconds) */
    TimeDuration();

    /** Constructor */
    TimeDuration(const boost::posix_time::time_duration& duration);

    /** Convert to string */
    operator std::string() const;

    /** Addition operator */
    TimeDuration operator +(const TimeDuration& b) const;

    /** Subtraction operator */
    TimeDuration operator -(const TimeDuration& b) const;

    /** Unary minus operator */
    TimeDuration operator -() const;

    /** Divide the time duration */
    TimeDuration operator /(const double& b) const;

    /** Divide the time duration */
    TimeDuration operator /(int b) const;

    /** Multiply the time duration */
    TimeDuration operator *(const double& b) const;

    /** Multiply the time duration */
    TimeDuration operator *(int b) const;

    /** Divide the datetime */
    double operator /(const TimeDuration& b) const;

    /** Get the total number of minutes */
    long total_minutes() const;
};

/** Null time duration.
Valid time duration, 0 seconds.
*/
const TimeDuration TD_NULL = boost::posix_time::seconds(0);

/** One second */
const TimeDuration SECOND = boost::posix_time::seconds(1);

/** One minute */
const TimeDuration MINUTE = SECOND * 60;

/** One hour */
const TimeDuration HOUR = MINUTE * 60;

/** One day */
const TimeDuration DAY = HOUR * 24;

/** One week */
const TimeDuration WEEK = DAY * 7;

/** Return time duration between two datetimes */
TimeDuration operator -(const WDateTime& a, const WDateTime& b);

/** Increase the datetime by the time duration */
WDateTime operator +(const WDateTime& a, const TimeDuration& b);

/** Decrease the datetime by the time duration */
WDateTime operator -(const WDateTime& a, const TimeDuration& b);

/** Increase the datetime by the time duration */
WDateTime& operator +=(WDateTime& a, const TimeDuration& b);

/** Decrease the datetime by the time duration */
WDateTime& operator -=(WDateTime& a, const TimeDuration& b);

/** Multiply the datetime */
TimeDuration operator *(const double& b, const TimeDuration& a);

/** Return random time duration from open interval [min, max).

\ingroup rand
*/
TimeDuration rand_range(const TimeDuration& start, const TimeDuration& stop);

}

}

}

#endif

