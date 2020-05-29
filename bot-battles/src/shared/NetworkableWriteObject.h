#ifndef __NETWORKABLE_WRITE_OBJECT_H__
#define __NETWORKABLE_WRITE_OBJECT_H__

namespace sand {

class OutputMemoryStream;

//----------------------------------------------------------------------------------------------------
struct NetworkableWriteObject {

    virtual ~NetworkableWriteObject() = default;

    virtual U64 Write(OutputMemoryStream& outputStream, U64 dirtyState) const = 0;
};
}

#endif
