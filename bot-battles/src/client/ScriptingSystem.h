#ifndef __SCRIPTING_SYSTEM_H__
#define __SCRIPTING_SYSTEM_H__

#include "System.h"

#include <embed.h>
#include <pybind11.h>

namespace py = pybind11;

namespace sand {

//----------------------------------------------------------------------------------------------------
class ScriptingSystem : public System, public Subject, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::SCRIPTING;
    }

public:
    ScriptingSystem();

    bool StartUp() override;
    bool PreUpdate() override;
    bool Update() override;

    void OnNotify(const Event& event) override;

private:
    void ImportScripts() const;
    void InitScripts() const;

    void OnCollisionEnter(Entity entityA, Entity entityB, glm::vec2 linearVelocityA, glm::vec2 linearVelocityB, glm::vec2 normal) const;
    void OnSeenNewEntity(Entity entity) const;
    void OnSeenLostEntity(Entity entity) const;
    void OnWeaponHit(Entity entity) const;
    void OnWeaponMissed(Entity entity) const;
    void OnWeaponPrimaryPickedUp(Entity entity) const;
    void OnWeaponPrimaryReloaded(Entity entity, U32 ammo) const;
    void OnHealthHurt(Entity entity, U32 health) const;
    void OnHealthHealed(Entity entity, U32 health) const;
    void OnHealthPickedUp(Entity entity) const;
};
}

#endif
