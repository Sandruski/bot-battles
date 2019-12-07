#include "DeliveryManagerClient.h"

#include "MemoryStream.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
DeliveryManagerClient::DeliveryManagerClient()
    : m_pendingAcks()
    , m_nextOutgoingSequenceNumber(0)
    , m_nextExpectedSequenceNumber(0)
    , m_deliveriesCount(0)
{
}

//----------------------------------------------------------------------------------------------------
DeliveryManagerClient::~DeliveryManagerClient()
{
}

//----------------------------------------------------------------------------------------------------
void DeliveryManagerClient::WriteState(OutputMemoryStream& outputStream)
{
    WriteSequenceNumber(outputStream);
    WritePendingAcks(outputStream);
}

//----------------------------------------------------------------------------------------------------
bool DeliveryManagerClient::ReadState(InputMemoryStream& inputStream)
{
    return ReadSequenceNumber(inputStream);
}

//----------------------------------------------------------------------------------------------------
void DeliveryManagerClient::WriteSequenceNumber(OutputMemoryStream& outputStream)
{
    SequenceNumber sequenceNumber = m_nextOutgoingSequenceNumber++;
    outputStream.Write(sequenceNumber);

    ++m_deliveriesCount;
}

//----------------------------------------------------------------------------------------------------
bool DeliveryManagerClient::ReadSequenceNumber(InputMemoryStream& inputStream)
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
void DeliveryManagerClient::AddPendingAck(SequenceNumber sequenceNumber)
{
    const bool hasPendingAcks = !m_pendingAcks.empty();
    const bool isConsecutive = hasPendingAcks ? m_pendingAcks.back().AddSequenceNumber(sequenceNumber) : false;
    if (!isConsecutive) {
        m_pendingAcks.emplace_back(sequenceNumber);
    }
}

//----------------------------------------------------------------------------------------------------
void DeliveryManagerClient::WritePendingAcks(OutputMemoryStream& outputStream)
{
    const bool hasPendingAcks = !m_pendingAcks.empty();
    outputStream.Write(hasPendingAcks);
    if (hasPendingAcks) {
        m_pendingAcks.front().Write(outputStream);
        m_pendingAcks.pop_front();
    }
}
}
