/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * wt-classes is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef WT_UTIL_HPP_
#define WT_UTIL_HPP_

#include <boost/cast.hpp>

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

}

}

#endif

