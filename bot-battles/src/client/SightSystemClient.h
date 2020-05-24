#ifndef __SIGHT_SYSTEM_CLIENT_H__
#define __SIGHT_SYSTEM_CLIENT_H__

#include "SightSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class SightSystemClient : public SightSystem, public Subject, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::SIGHT;
    }

public:
    SightSystemClient();

    bool Render() override;

    void OnNotify(const Event& event) override;

private:
    void OnSeenNewEntity(Entity seenEntity) const;
    void OnSeenLostEntity(Entity seenEntity) const;
};
}

#endif
