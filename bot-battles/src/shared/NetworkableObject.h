#ifndef __NETWORKABLE_OBJECT_H__
#define __NETWORKABLE_OBJECT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
struct NetworkableObject {

    virtual ~NetworkableObject() = default;

    virtual U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const = 0;
    virtual void Read(InputMemoryStream& inputStream, U32 dirtyState) = 0;
};
}

#endif
