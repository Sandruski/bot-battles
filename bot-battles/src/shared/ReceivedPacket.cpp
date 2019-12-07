#include "ReceivedPacket.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
ReceivedPacket::ReceivedPacket(const InputMemoryStream& buffer, const SocketAddress& fromAddress, F32 timestamp)
    : m_buffer(buffer)
    , m_fromAddress(fromAddress)
    , m_timestamp(timestamp)
{
}

//----------------------------------------------------------------------------------------------------
ReceivedPacket::~ReceivedPacket()
{
}
}
