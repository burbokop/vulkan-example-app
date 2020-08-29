#ifndef TIME_H
#define TIME_H

#include <inttypes.h>

namespace e172vp {

struct Time {
    typedef int64_t time_t;

    static time_t currentMilliseconds();
};

}
#endif // TIME_H
