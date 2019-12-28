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
Delivery::~Delivery()
{
}
}