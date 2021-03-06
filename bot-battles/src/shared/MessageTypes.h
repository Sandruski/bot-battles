#ifndef __MESSAGE_TYPES_H__
#define __MESSAGE_TYPES_H__

namespace sand {

enum class ClientMessageType : U16 {

    HELLO,
    REHELLO,
    BYE,

    INPUT,

    COUNT
};

enum class ServerMessageType : U16 {

    WELCOME,
    UNWELCOME,
    REWELCOME,
    PLAY,
    RESULTS,
    BYE,

    STATE,

    COUNT
};
}

#endif
