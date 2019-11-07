#ifndef __NET_COMPONENT_H__
#define __NET_COMPONENT_H__

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
struct NetComponentWrite {
    virtual void Write(OutputMemoryStream& outputStream, U16 memberFlags) const = 0;
};

//----------------------------------------------------------------------------------------------------
struct NetComponentRead {
    virtual void Read(InputMemoryStream& inputStream) = 0;
};
}

#endif
