#include "Delivery.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Delivery::Delivery(SequenceNumber sequenceNumber)
    : m_replicationResultManager(nullptr)
    , m_sequenceNumber(sequenceNumber)
    , m_timestamp(Time::GetInstance().GetTime())
{
}

//----------------------------------------------------------------------------------------------------
SequenceNumber Delivery::GetSequenceNumber() const
{
    return m_sequenceNumber;
}

//----------------------------------------------------------------------------------------------------
F32 Delivery::GetTimestamp() const
{
    return m_timestamp;
}
}
