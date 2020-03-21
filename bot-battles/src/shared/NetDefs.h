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

const U32 MAX_PACKET_SIZE = 10000; //1470
const U32 MAX_PACKETS_PER_FRAME = 10;
const U32 MAX_INPUTS_PER_PACKET = 30;

const U32 MAX_FRAMES = 256;

using PlayerID = U32;
const U32 MAX_PLAYER_IDS = 2; // MAX_CLIENTS
const PlayerID INVALID_PLAYER_ID = MAX_PLAYER_IDS;

using NetworkID = U32;
const U32 MAX_NETWORK_IDS = 128; // MAX_OBJECTS_PER_CLIENT
const NetworkID INVALID_NETWORK_ID = MAX_NETWORK_IDS;

using SequenceNumber = U16;
const SequenceNumber INVALID_SEQUENCE_NUMBER = UINT16_MAX;

const F32 ENTITY_INTERPOLATION_PERIOD = 1.0f / FPS_SERVER; // TODO: *2? https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking#Entity_interpolation

const F32 DISCONNECT_TIMEOUT = 15.0f; // 5.0f
#ifdef _SERVER
const F32 ACK_TIMEOUT = 5.0f; // 0.5f
#endif
}

#endif
