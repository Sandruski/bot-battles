#ifndef __PLAYER_COMPONENT_H__
#define __PLAYER_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct PlayerComponent : public Component {
    static ComponentType GetType() { return ComponentType::PLAYER; }
};
}

#endif
