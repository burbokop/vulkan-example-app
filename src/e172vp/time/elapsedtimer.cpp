#include "elapsedtimer.h"

#include "time.h"

namespace e172vp {

ElapsedTimer::time_t ElapsedTimer::interval() const {
    return m_interval;
}

void ElapsedTimer::setInterval(const time_t &interval)
{
    m_interval = interval;
}

ElapsedTimer::ElapsedTimer(time_t interval) {
    m_interval = interval;
    reset();
}

bool ElapsedTimer::check(bool condition) {
    if(m_interval > 0) {
        const auto milliseconds = Time::currentMilliseconds();
        if (milliseconds - m_checkPoint > m_interval && condition) {
            m_checkPoint = milliseconds;
            return true;
        }
        return false;
    }
    return true;
}

ElapsedTimer::time_t ElapsedTimer::elapsed() {
    return Time::currentMilliseconds() - m_startPoint;
}

void ElapsedTimer::reset() {
    m_startPoint = Time::currentMilliseconds();
    m_checkPoint = m_startPoint;
}

double ElapsedTimer::progress() {
    const double milliseconds = Time::currentMilliseconds();
    if(m_interval == 0)\
        return 0;
    return (milliseconds - m_checkPoint) / m_interval;
}

}
