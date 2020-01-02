#ifndef __NETWORKABLE_READ_OBJECT_H__
#define __NETWORKABLE_READ_OBJECT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
struct NetworkableReadObject {

    virtual ~NetworkableReadObject() = default;

    virtual void Read(InputMemoryStream& inputStream, U32 dirtyState, bool isLocalPlayer) = 0;
};
}

#endif
