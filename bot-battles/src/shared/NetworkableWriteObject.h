#ifndef __NETWORKABLE_WRITE_OBJECT_H__
#define __NETWORKABLE_WRITE_OBJECT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
struct NetworkableWriteObject {

    virtual ~NetworkableWriteObject() = default;

    virtual U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const = 0;
};
}

#endif
