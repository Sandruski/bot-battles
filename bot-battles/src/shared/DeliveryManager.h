#ifndef __DELIVERY_MANAGER_H__
#define __DELIVERY_MANAGER_H__

#include "AckRange.h"
#include "Delivery.h"
#include "Subject.h"

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
class DeliveryManager : public Subject {
public:
    DeliveryManager();
    ~DeliveryManager();

    Delivery& WriteState(OutputMemoryStream& outputStream);
    bool ReadState(InputMemoryStream& inputStream);

    void ProcessTimedOutPackets();

private:
    Delivery& WriteSequenceNumber(OutputMemoryStream& outputStream);
    bool ReadSequenceNumber(InputMemoryStream& inputStream);

    void AddPendingAck(SequenceNumber sequenceNumber);
    void WritePendingAcks(OutputMemoryStream& outputStream);
    void ReadPendingAcks(InputMemoryStream& inputStream);

    void HandleDeliverySuccess();
    void HandleDeliveryFailure();

private:
    std::deque<Delivery> m_deliveries;
    std::deque<AckRange> m_pendingAcks;

    SequenceNumber m_nextOutgoingSequenceNumber;
    SequenceNumber m_nextExpectedSequenceNumber;

    U32 m_deliveriesCount;
    U32 m_successfulDeliveriesCount;
    U32 m_failedDeliveriesCount;
};
}

#endif
