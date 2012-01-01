/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WT_UTIL_HPP_
#define WT_UTIL_HPP_

#include <boost/cast.hpp>

#include <Wt/WGlobal>

namespace Wt {

namespace Wc {

/** Return whether an object is an instance of a class or of a subclass thereof.
This function and it's documentation was copied from python.
*/
template <class T, class S>
bool isinstance(const S* object) {
    try {
        return dynamic_cast<const T*>(object) != 0;
    } catch (...) {
        return false;
    }
}

#define downcast boost::polymorphic_downcast

/** Call triggerUpdate() in current WApplication */
void updates_trigger();

/** Post triggerUpdate() to the application */
void updates_poster(WServer* server, WApplication* app);

}

}

#endif

