/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_TIME_DURATION_DBO_HPP_
#define WC_TIME_DURATION_DBO_HPP_

#include <Wt/WGlobal>

#include "TimeDuration.hpp"

namespace Wt {

namespace Dbo {

/** Dbo SQL value traits specification for TimeDuration class.

\ingroup time
*/
template<>
struct sql_value_traits<Wc::td::TimeDuration, void> {
    static const bool specialized = true;

    static const char* type(SqlConnection* conn, int size);
    static void bind(const Wc::td::TimeDuration& v, SqlStatement* statement,
                     int column, int size);
    static bool read(Wc::td::TimeDuration& v, SqlStatement* statement,
                     int column, int size);
};

}

}

#endif

