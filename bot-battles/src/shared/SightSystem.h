#ifndef __SIGHT_SYSTEM_H__
#define __SIGHT_SYSTEM_H__

#include "System.h"

namespace sand {

struct TransformComponent;
struct SightComponent;

//----------------------------------------------------------------------------------------------------
class SightSystem : public System {

public:
    void Draw(std::weak_ptr<TransformComponent> transformComponent, std::weak_ptr<SightComponent> sightComponent) const;
};
}

#endif
