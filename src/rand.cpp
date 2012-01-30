/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <climits>

#include <Wt/WRandom>

#include "rand.hpp"

namespace Wt {

namespace Wc {

const unsigned int UINT_MIN = 0;

unsigned int rand() {
    return WRandom::get();
}

unsigned int rr(unsigned int stop) {
    return rand() % stop;
}

unsigned int rr(unsigned int start, unsigned int stop) {
    return start + rand() % (stop - start);
}

unsigned int rr(unsigned int start, unsigned int stop, unsigned int step) {
    return start + (rand() % ((stop - start) / step)) * step;
}

double drr(double start, double stop) {
    double stop_start = stop - start;
    const double max_min = UINT_MAX - UINT_MIN;
    return start + double(rand()) / max_min * stop_start;
}

ptrdiff_t rand_for_shuffle(ptrdiff_t i) {
    return rr(static_cast<unsigned int>(i));
}

}

}

