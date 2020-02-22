#ifndef __REMOTE_PLAYER_COMPONENT_H__
#define __REMOTE_PLAYER_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct RemotePlayerComponent : public Component {
    static ComponentType GetType() { return ComponentType::REMOTE_PLAYER; }
};
}

#endif
