#include "DeliveryManagerServer.h"

#include "AckRange.h"
#include "ReplicationResultManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
DeliveryManagerServer::DeliveryManagerServer()
    : m_deliveries()
    , m_nextOutgoingSequenceNumber(0)
    , m_nextExpectedSequenceNumber(0)
    , m_deliveriesCount(0)
    , m_successfulDeliveriesCount(0)
    , m_failedDeliveriesCount(0)
{
}

//----------------------------------------------------------------------------------------------------
Delivery& DeliveryManagerServer::WriteState(OutputMemoryStream& outputStream)
{
    return WriteSequenceNumber(outputStream);
}

//----------------------------------------------------------------------------------------------------
bool DeliveryManagerServer::ReadState(InputMemoryStream& inputStream)
{
    const bool ret = ReadSequenceNumber(inputStream);
    ReadPendingAcks(inputStream);

    return ret;
}

//----------------------------------------------------------------------------------------------------
void DeliveryManagerServer::ProcessTimedOutPackets()
{
    while (!m_deliveries.empty()) {

        const Delivery& delivery = m_deliveries.front();
        F32 timestamp = delivery.GetTimestamp();
        F32 time = Time::GetInstance().GetTime();
        F32 timeout = time - timestamp;
        if (timeout >= ACK_TIMEOUT) {
            Delivery deliveryCopy = delivery;
            m_deliveries.pop_front();
            ILOG("Timeout failure");
            HandleDeliveryFailure(deliveryCopy);
        } else {
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------
const std::deque<Delivery>& DeliveryManagerServer::GetDeliveries() const
{
    return m_deliveries;
}

//----------------------------------------------------------------------------------------------------
Delivery& DeliveryManagerServer::WriteSequenceNumber(OutputMemoryStream& outputStream)
{
    SequenceNumber sequenceNumber = m_nextOutgoingSequenceNumber++;
    outputStream.Write(sequenceNumber);

    ++m_deliveriesCount;

    return m_deliveries.emplace_back(sequenceNumber);
}

//----------------------------------------------------------------------------------------------------
bool DeliveryManagerServer::ReadSequenceNumber(InputMemoryStream& inputStream)
{
    SequenceNumber sequenceNumber = INVALID_SEQUENCE_NUMBER;
    inputStream.Read(sequenceNumber);
    if (sequenceNumber >= m_nextExpectedSequenceNumber) {
        m_nextExpectedSequenceNumber = sequenceNumber + 1;
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------------------------------
void DeliveryManagerServer::ReadPendingAcks(InputMemoryStream& inputStream)
{
    bool hasPendingAcks = false;
    inputStream.Read(hasPendingAcks);
    if (hasPendingAcks) {
        AckRange ackRange;
        ackRange.Read(inputStream);

        SequenceNumber nextAckdSequenceNumber = ackRange.GetStartSequenceNumber();
        ILOG("Next ackd sequence number is %u", nextAckdSequenceNumber);
        SequenceNumber lastAckdSequenceNumber = nextAckdSequenceNumber + static_cast<SequenceNumber>(ackRange.GetCount()) - 1;
        ILOG("Count is %u", ackRange.GetCount());
        ILOG("Last ackd sequence number is %u", lastAckdSequenceNumber);
        while (nextAckdSequenceNumber <= lastAckdSequenceNumber && !m_deliveries.empty()) {
            const Delivery& delivery = m_deliveries.front();
            SequenceNumber sequenceNumber = delivery.GetSequenceNumber();
            if (sequenceNumber == nextAckdSequenceNumber) {
                HandleDeliverySuccess(delivery);
                m_deliveries.pop_front();
                ILOG("Delivery success because sequenceNumber %u == %u nextAckdSequenceNumber", sequenceNumber, nextAckdSequenceNumber);
                ++nextAckdSequenceNumber;
            } else if (sequenceNumber < nextAckdSequenceNumber) {
                Delivery deliveryCopy = delivery;
                m_deliveries.pop_front();
                HandleDeliveryFailure(deliveryCopy);
                ILOG("Normal failure. Sequence number %u < %u", sequenceNumber, nextAckdSequenceNumber);
            } else {
                ++nextAckdSequenceNumber;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void DeliveryManagerServer::HandleDeliverySuccess(const Delivery& delivery)
{
    delivery.m_replicationResultManager->HandleDeliverySuccess(*this);

    ++m_successfulDeliveriesCount;
}

//----------------------------------------------------------------------------------------------------
void DeliveryManagerServer::HandleDeliveryFailure(const Delivery& delivery)
{
    delivery.m_replicationResultManager->HandleDeliveryFailure(*this);

    ++m_failedDeliveriesCount;
}
}
