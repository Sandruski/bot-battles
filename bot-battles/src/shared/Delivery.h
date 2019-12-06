#ifndef __DELIVERY_H__
#define __DELIVERY_H__

namespace sand {

class ReplicationDeliveryManagerServer;

//----------------------------------------------------------------------------------------------------
class Delivery {
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

public:
    std::shared_ptr<ReplicationDeliveryManagerServer> m_replicationDeliveryManager;

private:
    SequenceNumber m_sequenceNumber;
    F32 m_timestamp;
};
}

#endif
