#ifndef __RECEIVED_PACKET_H__
#define __RECEIVED_PACKET_H__

#include "MemoryStream.h"
#include "SocketAddress.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ReceivedPacket {
public:
    ReceivedPacket(const InputMemoryStream& buffer, const SocketAddress& fromAddress, F32 timestamp);
    ~ReceivedPacket();

    InputMemoryStream& GetBuffer()
    {
        return m_buffer;
    }

    const SocketAddress& GetFromAddress() const
    {
        return m_fromAddress;
    }

    F32 GetTimestamp() const
    {
        return m_timestamp;
    }

private:
    InputMemoryStream m_buffer;
    SocketAddress m_fromAddress;
    F32 m_timestamp;
};
}

#endif
