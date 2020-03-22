#ifndef __MY_TIMER_H__
#define __MY_TIMER_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class MyTimer {
public:
    MyTimer();

    void Start();

    F64 ReadSec() const;
    F64 ReadMs() const;

private:
    U64 m_startCounter;
    U64 m_frequency;
};
}

#endif
