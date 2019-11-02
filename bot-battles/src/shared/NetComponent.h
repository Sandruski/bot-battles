#ifndef __NET_COMPONENT_H__
#define __NET_COMPONENT_H__

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
struct NetComponentWrite {
    virtual void Write(OutputMemoryStream& stream, U16 members) const = 0;
};

//----------------------------------------------------------------------------------------------------
struct NetComponentRead {
    virtual void Read(InputMemoryStream& stream) = 0;
};
}

#endif
