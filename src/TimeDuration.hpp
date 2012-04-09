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

class WDateTime; // not declared n WGlobal of Wt 3.1.2

namespace Wc {

/** \defgroup time Time-related
Classes and typedef simplifying time management.
*/

/** Namespace for time duration constants and integration with WDateTime.
\ingroup time
*/
namespace td {

/** Time duration.

To integrate this type into Wt::Dbo see
Wt::Dbo::sql_value_traits< Wc::td::TimeDuration, void >.

\ingroup time
*/
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

\ingroup time
*/
const TimeDuration TD_NULL = boost::posix_time::seconds(0);

/** One second.

\ingroup time
*/
const TimeDuration SECOND = boost::posix_time::seconds(1);

/** One minute.

\ingroup time
*/
const TimeDuration MINUTE = SECOND * 60;

/** One hour.

\ingroup time
*/
const TimeDuration HOUR = MINUTE * 60;

/** One day.

\ingroup time
*/
const TimeDuration DAY = HOUR * 24;

/** One week.

\ingroup time
*/
const TimeDuration WEEK = DAY * 7;

/** Return time duration between two datetimes.

\ingroup time
*/
TimeDuration operator -(const WDateTime& a, const WDateTime& b);

/** Increase the datetime by the time duration.

\ingroup time
*/
WDateTime operator +(const WDateTime& a, const TimeDuration& b);

/** Decrease the datetime by the time duration.

\ingroup time
*/
WDateTime operator -(const WDateTime& a, const TimeDuration& b);

/** Increase the datetime by the time duration.

\ingroup time
*/
WDateTime& operator +=(WDateTime& a, const TimeDuration& b);

/** Decrease the datetime by the time duration.

\ingroup time
*/
WDateTime& operator -=(WDateTime& a, const TimeDuration& b);

/** Multiply the datetime.

\ingroup time
*/
TimeDuration operator *(const double& b, const TimeDuration& a);

/** Return random time duration from open interval [min, max).

\ingroup time
*/
TimeDuration rand_range(const TimeDuration& start, const TimeDuration& stop);

}

}

}

#endif

