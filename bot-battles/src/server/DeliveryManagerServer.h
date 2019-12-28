#ifndef __DELIVERY_MANAGER_SERVER_H__
#define __DELIVERY_MANAGER_SERVER_H__

#include "AckRange.h"
#include "Delivery.h"

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
class DeliveryManagerServer {
public:
    DeliveryManagerServer();
    ~DeliveryManagerServer();

    Delivery& WriteState(OutputMemoryStream& outputStream);
    bool ReadState(InputMemoryStream& inputStream);

    void ProcessTimedOutPackets();

    const std::deque<Delivery>& GetDeliveries() const
    {
        return m_deliveries;
    }

private:
    Delivery& WriteSequenceNumber(OutputMemoryStream& outputStream);
    bool ReadSequenceNumber(InputMemoryStream& inputStream);

    void ReadPendingAcks(InputMemoryStream& inputStream);

    void HandleDeliverySuccess(const Delivery& delivery);
    void HandleDeliveryFailure(const Delivery& delivery);

private:
    std::deque<Delivery> m_deliveries;

    SequenceNumber m_nextOutgoingSequenceNumber;
    SequenceNumber m_nextExpectedSequenceNumber;

    U32 m_deliveriesCount;
    U32 m_successfulDeliveriesCount;
    U32 m_failedDeliveriesCount;
};
}

#endif