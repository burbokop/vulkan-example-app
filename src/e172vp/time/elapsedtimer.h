#ifndef ELAPSEDTIMER_H
#define ELAPSEDTIMER_H

#include <inttypes.h>

namespace e172vp {



class ElapsedTimer {
public:
    typedef int64_t time_t;
private:
    time_t m_interval;
    time_t m_startPoint;
    time_t m_checkPoint;
public:
    ElapsedTimer(time_t interval = 0);
    bool check(bool condition = true);
    time_t elapsed();
    void reset();
    double progress();

    time_t interval() const;
    void setInterval(const time_t &interval);
};

}
#endif // ELAPSEDTIMER_H
