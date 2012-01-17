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

/** Time duartion */
typedef boost::posix_time::time_duration TimeDuration;

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

/** Convert time duration to string */
std::string td2str(const TimeDuration& td);

/** Return time duration between two datetimes */
TimeDuration operator -(const Wt::WDateTime& a, const Wt::WDateTime& b);

/** Increase the datetime by the time duration */
Wt::WDateTime operator +(const Wt::WDateTime& a, const TimeDuration& b);

/** Decrease the datetime by the time duration */
Wt::WDateTime operator -(const Wt::WDateTime& a, const TimeDuration& b);

/** Increase the datetime by the time duration */
Wt::WDateTime& operator +=(Wt::WDateTime& a, const TimeDuration& b);

/** Decrease the datetime by the time duration */
Wt::WDateTime& operator -=(Wt::WDateTime& a, const TimeDuration& b);

/** Divide the datetime */
TimeDuration operator /(const TimeDuration& a, const double& b);

/** Multiply the datetime */
TimeDuration operator *(const TimeDuration& a, const double& b);

/** Multiply the datetime */
TimeDuration operator *(const double& b, const TimeDuration& a);

/** Divide the datetime */
double operator /(const TimeDuration& a, const TimeDuration& b);

/** Current datetime */
Wt::WDateTime now();

/** Get the total number of minutes */
long total_minutes(const TimeDuration& t);

/** Return random time duration from open interval [min, max).

\ingroup util
*/
TimeDuration rand_range(const TimeDuration& start, const TimeDuration& stop);

}

}

}

#endif

