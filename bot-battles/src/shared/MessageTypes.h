#ifndef __MESSAGE_TYPES_H__
#define __MESSAGE_TYPES_H__

namespace sand {

enum class ClientMessageType : U16 {

    HELLO,
    INPUT,
    AGAIN,

    BYE,

    COUNT
};

enum class ServerMessageType : U16 {

    WELCOME,
    STATE,
    RESULT,

    BYE,

    COUNT
};
}

#endif
