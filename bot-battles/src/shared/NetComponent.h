#ifndef __NET_COMPONENT_H__
#define __NET_COMPONENT_H__

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
struct WriteNetComponent {
    virtual void Write(OutputMemoryStream& outputStream, U16 memberFlags) const = 0;
};

//----------------------------------------------------------------------------------------------------
struct ReadNetComponent {
    virtual void Read(InputMemoryStream& inputStream, U16 memberFlags) = 0;
};
}

#endif
