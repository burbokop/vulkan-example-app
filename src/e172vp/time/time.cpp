#include "time.h"
#include <sys/timeb.h>


namespace e172vp {

Time::time_t Time::currentMilliseconds() {
    timeb tb;
    ftime(&tb);
    unsigned nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
    return nCount;
}


}
