#ifndef __COMPONENT_H__
#define __COMPONENT_H__

namespace sand {

class OutputMemoryStream;

//----------------------------------------------------------------------------------------------------
struct NetComponent {
    virtual void Write(OutputMemoryStream& stream, U32 members) const = 0;
    virtual void Read(InputMemoryStream& stream) = 0;
};
}

#endif
