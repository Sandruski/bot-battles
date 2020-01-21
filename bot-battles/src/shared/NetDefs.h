#ifndef __NET_DEFS_H__
#define __NET_DEFS_H__

namespace sand {

enum class Endianness {

    LITTLE_ENDIAN, // Intel's x86, Intel's x64, Apple's iOS
    BIG_ENDIAN // Xbox 360, PlayStation 3, IBM's PowerPC
};

const Endianness STREAM_ENDIANNESS = Endianness::LITTLE_ENDIAN;

inline const Endianness PLATFORM_ENDIANNESS()
{
    short word = 0x1;
    char* ptr = reinterpret_cast<char*>(&word);
    return ptr[0] ? Endianness::LITTLE_ENDIAN : Endianness::BIG_ENDIAN;
}

const U32 MAX_PACKET_SIZE = 1470;
const U32 MAX_PACKETS_PER_FRAME = 10;
const U32 MAX_MOVES_PER_PACKET = 3;

using PlayerID = U32;
const U32 MAX_PLAYER_IDS = 2; // MAX_CLIENTS
const PlayerID INVALID_PLAYER_ID = MAX_PLAYER_IDS;

using NetworkID = U32;
const U32 MAX_NETWORK_IDS = 32; // MAX_OBJECTS_PER_CLIENT
const NetworkID INVALID_NETWORK_ID = MAX_NETWORK_IDS;

using SequenceNumber = U16;
const SequenceNumber INVALID_SEQUENCE_NUMBER = UINT16_MAX;

#ifdef _CLIENT
const F32 TIME_BETWEEN_MOVE = 0.0f;
#elif defined(_SERVER)
const F32 ACK_TIMEOUT = 0.5f;
const F32 DISCONNECT_TIMEOUT = 5.0f;
#endif
}

#endif
