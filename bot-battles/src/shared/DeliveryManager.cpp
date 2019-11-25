#include "DeliveryManager.h"

#include "MemoryStream.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
DeliveryManager::DeliveryManager()
    : m_deliveries()
    , m_pendingAcks()
    , m_nextOutgoingSequenceNumber(0)
    , m_nextExpectedSequenceNumber(0)
    , m_deliveriesCount(0)
    , m_successfulDeliveriesCount(0)
    , m_failedDeliveriesCount(0)
{
}

//----------------------------------------------------------------------------------------------------
DeliveryManager::~DeliveryManager()
{
}

//----------------------------------------------------------------------------------------------------
Delivery& DeliveryManager::WriteState(OutputMemoryStream& outputStream)
{
    Delivery& delivery = WriteSequenceNumber(outputStream);
    WritePendingAcks(outputStream);

    return delivery;
}

//----------------------------------------------------------------------------------------------------
bool DeliveryManager::ReadState(InputMemoryStream& inputStream)
{
    const bool ret = ReadSequenceNumber(inputStream);
    ReadPendingAcks(inputStream);

    return ret;
}

//----------------------------------------------------------------------------------------------------
void DeliveryManager::ProcessTimedOutPackets()
{
    F32 timeout = Time::GetInstance().GetTime() - ACK_TIMEOUT;
    while (!m_deliveries.empty()) {
        const Delivery& delivery = m_deliveries.front();
        F32 timestamp = delivery.GetTimestamp();
        if (timestamp < timeout) {
            HandleDeliveryFailure();
            m_deliveries.pop_front();
        } else {
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------
Delivery& DeliveryManager::WriteSequenceNumber(OutputMemoryStream& outputStream)
{
    SequenceNumber sequenceNumber = m_nextOutgoingSequenceNumber++;
    outputStream.Write(sequenceNumber);

    ++m_deliveriesCount;

    return m_deliveries.emplace_back(sequenceNumber);
}

//----------------------------------------------------------------------------------------------------
bool DeliveryManager::ReadSequenceNumber(InputMemoryStream& inputStream)
{
    SequenceNumber sequenceNumber = INVALID_SEQUENCE_NUMBER;
    inputStream.Read(sequenceNumber);
    if (sequenceNumber >= m_nextExpectedSequenceNumber) {
        AddPendingAck(sequenceNumber);
        m_nextExpectedSequenceNumber = sequenceNumber + 1;
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------------------------------
void DeliveryManager::AddPendingAck(SequenceNumber sequenceNumber)
{
    const bool hasPendingAcks = !m_pendingAcks.empty();
    const bool isConsecutive = hasPendingAcks ? m_pendingAcks.back().AddSequenceNumber(sequenceNumber) : false;
    if (!isConsecutive) {
        m_pendingAcks.emplace_back(sequenceNumber);
    }
}

//----------------------------------------------------------------------------------------------------
void DeliveryManager::WritePendingAcks(OutputMemoryStream& outputStream)
{
    const bool hasPendingAcks = !m_pendingAcks.empty();
    outputStream.Write(hasPendingAcks);
    if (hasPendingAcks) {
        m_pendingAcks.front().Write(outputStream);
        m_pendingAcks.pop_front();
    }
}

//----------------------------------------------------------------------------------------------------
void DeliveryManager::ReadPendingAcks(InputMemoryStream& inputStream)
{
    bool hasPendingAcks = false;
    inputStream.Read(hasPendingAcks);
    if (hasPendingAcks) {
        AckRange ackRange;
        ackRange.Read(inputStream);

        SequenceNumber sequenceNumber = ackRange.GetStartSequenceNumber();
        SequenceNumber lastSequenceNumber = sequenceNumber + static_cast<SequenceNumber>(ackRange.GetCount()) - 1;
        while (sequenceNumber <= lastSequenceNumber && !m_deliveries.empty()) {
            const Delivery& delivery = m_deliveries.front();
            SequenceNumber inFlightPacketSequenceNumber = delivery.GetSequenceNumber();
            if (inFlightPacketSequenceNumber >= sequenceNumber) {
                HandleDeliverySuccess();
                m_deliveries.pop_front();
                sequenceNumber = inFlightPacketSequenceNumber + 1;
            } else if (inFlightPacketSequenceNumber < sequenceNumber) {
                HandleDeliveryFailure();
                m_deliveries.pop_front();
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void DeliveryManager::HandleDeliverySuccess()
{
    Event newEvent;
    newEvent.eventType = EventType::DELIVERY_SUCCESS;
    NotifyEvent(newEvent);

    ++m_successfulDeliveriesCount;
}

//----------------------------------------------------------------------------------------------------
void DeliveryManager::HandleDeliveryFailure()
{
    Event newEvent;
    newEvent.eventType = EventType::DELIVERY_FAILURE;
    NotifyEvent(newEvent);

    ++m_failedDeliveriesCount;
}
}
