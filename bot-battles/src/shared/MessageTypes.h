#ifndef __MESSAGE_TYPES_H__
#define __MESSAGE_TYPES_H__

namespace sand {

enum class ClientMessageType : U16 {

    HELLO,
    REHELLO,
    INPUT,

    BYE,

    COUNT
};

enum class ServerMessageType : U16 {

    WELCOME,
    REWELCOME,
    STATE,
    RESULT,

    BYE,

    COUNT
};
}

#endif
