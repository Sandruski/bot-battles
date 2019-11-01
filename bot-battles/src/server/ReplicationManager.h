#ifndef __REPLICATION_MANAGER_H__
#define __REPLICATION_MANAGER_H__

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
enum class ReplicationAction {
    CREATE_ENTITY,
    UPDATE_ENTITY,
    REMOVE_ENTITY,

    COUNT,
    INVALID
};

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
class ReplicationHeader {
public:
    ReplicationHeader();
    ReplicationHeader(ReplicationAction replicationAction, NetworkID networkID);
    ~ReplicationHeader();

    void Write(OutputMemoryStream& stream);
    void Read(InputMemoryStream& stream);

    ReplicationAction GetAction() const
    {
        return m_replicationAction;
    }

    NetworkID GetNetworkID() const
    {
        return m_networkID;
    }

    U32 GetBitCount() const
    {
        return m_bitCount;
    }

private:
    ReplicationAction m_replicationAction;
    NetworkID m_networkID;
    U32 m_bitCount; // TODO: total size of the data
};

//----------------------------------------------------------------------------------------------------
class ReplicationManager {
public:
    ReplicationManager();
    ~ReplicationManager();

    void CreateEntityAction(OutputMemoryStream& stream, Entity entity) const;
    void UpdateEntityAction(OutputMemoryStream& stream, Entity entity) const;
    void RemoveEntityAction(OutputMemoryStream& stream, Entity entity) const;

    void ProcessAction(InputMemoryStream& stream) const;
    void CreateEntity(InputMemoryStream& stream, const ReplicationHeader& replicationHeader) const;
    void UpdateEntity(InputMemoryStream& stream, const ReplicationHeader& replicationHeader) const;
    void RemoveEntity(InputMemoryStream& stream, const ReplicationHeader& replicationHeader) const;
};
}

#endif
