#ifndef __REPLICATION_COMMAND_H__
#define __REPLICATION_COMMAND_H__

#include "EntityDefs.h"
#include "NetDefs.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
template <U32 value, U32 bits>
struct GetRequiredBitsHelper {
    enum { value = GetRequiredBitsHelper<(value >> 1), bits + 1>::value };
};

//----------------------------------------------------------------------------------------------------
template <U32 bits>
struct GetRequiredBitsHelper<0, bits> {
    enum { value = bits };
};

//----------------------------------------------------------------------------------------------------
template <U32 value>
struct GetRequiredBits {
    enum { value = GetRequiredBitsHelper<value, 0>::value };
};

// World state delta
// Object state delta
enum class ReplicationAction : U16 {
    CREATE_ENTITY,
    UPDATE_ENTITY,
    REMOVE_ENTITY,

    COUNT,
    INVALID
};

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
class ReplicationCommand {
public:
    ReplicationCommand();
    ReplicationCommand(ReplicationAction replicationAction);
    ~ReplicationCommand();

    void Write(OutputMemoryStream& stream);
    void Read(InputMemoryStream& stream);

    bool IsDirty() const
    {
        return m_replicationAction == ReplicationAction::REMOVE_ENTITY;
    }

    U32 GetBitCount() const
    {
        return m_bitCount;
    }

public:
    ReplicationAction m_replicationAction;
    U32 m_bitCount; // TODO: total size of the data
};
}

#endif
