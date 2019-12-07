#ifndef __DELIVERY_MANAGER_CLIENT_H__
#define __DELIVERY_MANAGER_CLIENT_H__

#include "AckRange.h"

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
class DeliveryManagerClient {
public:
    DeliveryManagerClient();
    ~DeliveryManagerClient();

    void WriteState(OutputMemoryStream& outputStream);
    bool ReadState(InputMemoryStream& inputStream);

private:
    void WriteSequenceNumber(OutputMemoryStream& outputStream);
    bool ReadSequenceNumber(InputMemoryStream& inputStream);

    void AddPendingAck(SequenceNumber sequenceNumber);
    void WritePendingAcks(OutputMemoryStream& outputStream);

private:
    std::deque<AckRange> m_pendingAcks;

    SequenceNumber m_nextOutgoingSequenceNumber;
    SequenceNumber m_nextExpectedSequenceNumber;

    U32 m_deliveriesCount;
};
}

#endif
