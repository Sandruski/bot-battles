#ifndef __DELIVERY_H__
#define __DELIVERY_H__

namespace sand {

class ReplicationResultManager;

//----------------------------------------------------------------------------------------------------
class Delivery {
public:
    Delivery(SequenceNumber sequenceNumber);

    SequenceNumber GetSequenceNumber() const;
    F32 GetTimestamp() const;

public:
    std::shared_ptr<ReplicationResultManager> m_replicationResultManager;

private:
    SequenceNumber m_sequenceNumber;
    F32 m_timestamp;
};
}

#endif
