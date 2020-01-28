#ifndef __NETWORKABLE_READ_OBJECT_H__
#define __NETWORKABLE_READ_OBJECT_H__

#include "ReplicationActionTypes.h"

namespace sand {

class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
struct NetworkableReadObject {

    virtual ~NetworkableReadObject() = default;

    virtual void Read(InputMemoryStream& inputStream, U32 dirtyState, ReplicationActionType replicationActionType, Entity entity) = 0;
};
}

#endif
