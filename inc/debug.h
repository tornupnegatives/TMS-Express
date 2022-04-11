//
// Created by Joseph Bellahcen on 4/6/22.
//

#ifndef TMS_EXPRESS_DEBUG_H
#define TMS_EXPRESS_DEBUG_H

#define DEBUG(fmt, ...) \
            do { if (_DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#include <cstdio>

#endif //TMS_EXPRESS_DEBUG_H
