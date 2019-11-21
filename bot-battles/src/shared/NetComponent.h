#ifndef __NET_COMPONENT_H__
#define __NET_COMPONENT_H__

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
struct NetComponent {
    virtual U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const = 0;
    virtual void Read(InputMemoryStream& inputStream, U32 dirtyState) = 0;
};
}

#endif
