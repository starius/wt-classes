/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cstddef>

namespace Wt {

namespace Wc {

/** Return random number.

\ingroup util
*/
unsigned int rand();

/** Return random number from open interval [0, stop).

\ingroup util
*/
unsigned int rr(unsigned int stop);

/** Return random number from open interval [start, stop).

\ingroup util
*/
unsigned int rr(unsigned int start, unsigned int stop);

/** Return random number from open interval [start, stop), with step step.
\code result = start + n * step \endcode

\ingroup util
*/
unsigned int rr(unsigned int start, unsigned int stop, unsigned int step);

/** Return random number from open interval [start, stop).

\ingroup util
*/
double drr(double start, double stop);

/** Return random number from open interval [0, i).
This function is useful for std::random_shuffle().
The value of the argument must be greater or equal to 0.

\ingroup util
*/
ptrdiff_t rand_for_shuffle(ptrdiff_t i);

}

}

