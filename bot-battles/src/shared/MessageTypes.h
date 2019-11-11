#ifndef __MESSAGE_TYPES_H__
#define __MESSAGE_TYPES_H__

namespace sand {

enum class ClientMessageType {

    HELLO,
    INPUT,
    GOODBYE,

    COUNT,
    INVALID
};

enum class ServerMessageType {

    WELCOME,
    STATE,

    COUNT,
    INVALID
};
}

#endif
