/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_RAND_HPP_
#define WC_RAND_HPP_

#include <cstddef>
#include <string>

namespace Wt {

namespace Wc {

/** \defgroup rand Random numbers
Functions to generate random variables.

\see rand_range()
*/

/** Return random number.

\ingroup rand
*/
unsigned int rr();

/** Return random number from open interval [0, stop).

\ingroup rand
*/
unsigned int rr(unsigned int stop);

/** Return random number from open interval [start, stop).

\ingroup rand
*/
unsigned int rr(unsigned int start, unsigned int stop);

/** Return random number from open interval [start, stop), with step step.
\code result = start + n * step \endcode

\ingroup rand
*/
unsigned int rr(unsigned int start, unsigned int stop, unsigned int step);

/** Return random number from open interval [start, stop).

\ingroup rand
*/
double drr(double start, double stop);

/** Return random number from open interval [0, i).
This function is useful for std::random_shuffle().
The value of the argument must be greater or equal to 0.

\ingroup rand
*/
ptrdiff_t rand_for_shuffle(ptrdiff_t i);

/** Return random string build from [a-zA-Z0-9].

\ingroup rand
*/
std::string rand_string(int length = 16);

}

}

#endif

