#ifndef __DELIVERY_MANAGER_H__
#define __DELIVERY_MANAGER_H__

#include "AckRange.h"
#include "Delivery.h"

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
class DeliveryManager {
public:
    DeliveryManager();
    ~DeliveryManager();

    Delivery& WriteSequenceNumber(OutputMemoryStream& outputStream);
    bool ReadSequenceNumber(InputMemoryStream& inputStream);

    void AddPendingAck(SequenceNumber sequenceNumber);
    void WritePendingAcks(OutputMemoryStream& outputStream);
    void ReadPendingAcks(InputMemoryStream& inputStream);

    void ProcessTimedOutPackets();

    void HandlePacketDeliverySuccess(const Delivery& delivery);
    void HandlePacketDeliveryFailure(const Delivery& delivery);

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
