#ifndef __DELIVERY_H__
#define __DELIVERY_H__

#include "Subject.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Delivery : public Subject {
public:
    Delivery(SequenceNumber sequenceNumber);
    ~Delivery();

    SequenceNumber GetSequenceNumber() const
    {
        return m_sequenceNumber;
    }

    F32 GetTimestamp() const
    {
        return m_timestamp;
    }

private:
    SequenceNumber m_sequenceNumber;
    F32 m_timestamp;
};
}

#endif
