#ifndef __MESSAGE_TYPES_H__
#define __MESSAGE_TYPES_H__

namespace sand {

enum class ClientMessageType : U16 {
    HELLO,
    INPUT,
    GOODBYE,

    COUNT,
    INVALID
};

enum class ServerMessageType : U16 {
    WELCOME,
    STATE,

    COUNT,
    INVALID
};
}

#endif
