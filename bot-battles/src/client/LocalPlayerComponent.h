#ifndef __LOCAL_PLAYER_COMPONENT_H__
#define __LOCAL_PLAYER_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct LocalPlayerComponent : public Component {
    static ComponentType GetType() { return ComponentType::LOCAL_PLAYER; }
};
}

#endif
