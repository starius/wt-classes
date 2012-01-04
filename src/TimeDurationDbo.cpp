/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WDateTime>
#include <Wt/Dbo/SqlConnection>
#include <Wt/Dbo/SqlStatement>
#include <Wt/Dbo/WtSqlTraits>

#include "TimeDurationDbo.hpp"

namespace Wt {

namespace Dbo {

const char* sql_value_traits<Wc::td::TimeDuration>::type(SqlConnection* conn,
        int /* size */) {
    return conn->dateTimeType(SqlTime);
}

void sql_value_traits<Wc::td::TimeDuration>::bind(const Wc::td::TimeDuration& v,
        SqlStatement* statement, int column, int /* size */) {
    statement->bind(column, v);
}

bool sql_value_traits<Wc::td::TimeDuration>::read(Wc::td::TimeDuration& v,
        SqlStatement* statement, int column, int /*size*/) {
    Wc::td::TimeDuration value;
    bool notNull = statement->getResult(column, &value);
    if (notNull) {
        v = value;
    }
    return notNull;
}

}

}

