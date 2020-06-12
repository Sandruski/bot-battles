#ifndef __SIGHT_COMPONENT_H__
#define __SIGHT_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

#ifdef _CLIENT
struct TransformComponent;
struct ColliderComponent;
struct RigidbodyComponent;
struct WeaponComponent;
struct HealthComponent;
struct BotComponent;
#endif

//----------------------------------------------------------------------------------------------------
// Entity Component
struct SightComponent : public Component
#ifdef _CLIENT
    ,
                        public NetworkableReadObject
#elif defined(_SERVER)
    ,
                        public NetworkableWriteObject
#endif
{
#ifdef _CLIENT
    struct SeenBotInfo {

        SeenBotInfo();

        // Python
        std::shared_ptr<TransformComponent> GetPyTransformComponent()
        {
            return m_transformComponent;
        }

        std::shared_ptr<ColliderComponent> GetPyColliderComponent()
        {
            return m_colliderComponent;
        }

        std::shared_ptr<RigidbodyComponent> GetPyRigidbodyComponent()
        {
            return m_rigidbodyComponent;
        }

        std::shared_ptr<WeaponComponent> GetPyWeaponComponent()
        {
            return m_weaponComponent;
        }

        std::shared_ptr<HealthComponent> GetPyHealthComponent()
        {
            return m_healthComponent;
        }

        std::shared_ptr<BotComponent> GetPyBotComponent()
        {
            return m_botComponent;
        }

        std::shared_ptr<TransformComponent> m_transformComponent;
        std::shared_ptr<ColliderComponent> m_colliderComponent;
        std::shared_ptr<RigidbodyComponent> m_rigidbodyComponent;
        std::shared_ptr<WeaponComponent> m_weaponComponent;
        std::shared_ptr<HealthComponent> m_healthComponent;
        std::shared_ptr<BotComponent> m_botComponent;
    };

    struct SeenWeaponInfo {

        SeenWeaponInfo();

        // Python
        std::shared_ptr<TransformComponent> GetPyTransformComponent()
        {
            return m_transformComponent;
        }

        std::shared_ptr<ColliderComponent> GetPyColliderComponent()
        {
            return m_colliderComponent;
        }

        std::shared_ptr<WeaponComponent> GetPyWeaponComponent()
        {
            return m_weaponComponent;
        }

        std::shared_ptr<TransformComponent> m_transformComponent;
        std::shared_ptr<ColliderComponent> m_colliderComponent;
        std::shared_ptr<WeaponComponent> m_weaponComponent;
    };

    struct SeenHealthInfo {

        SeenHealthInfo();

        // Python
        std::shared_ptr<TransformComponent> GetPyTransformComponent()
        {
            return m_transformComponent;
        }

        std::shared_ptr<ColliderComponent> GetPyColliderComponent()
        {
            return m_colliderComponent;
        }

        std::shared_ptr<HealthComponent> GetPyHealthComponent()
        {
            return m_healthComponent;
        }

        std::shared_ptr<TransformComponent> m_transformComponent;
        std::shared_ptr<ColliderComponent> m_colliderComponent;
        std::shared_ptr<HealthComponent> m_healthComponent;
    };
#endif

    static ComponentType GetType()
    {
        return ComponentType::SIGHT;
    }

    SightComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U64 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U64 Write(OutputMemoryStream& outputStream, U64 dirtyState) const override;
#endif

    bool IsEntitySeen(Entity entity);

#ifdef _CLIENT
    // Python
    std::vector<Entity> GetPySeenBotEntities() const;
    SeenBotInfo GetPySeenBotInfo(Entity entity) const;
    std::vector<Entity> GetPySeenWeaponEntities() const;
    SeenWeaponInfo GetPySeenWeaponInfo(Entity entity) const;
    std::vector<Entity> GetPySeenHealthEntities() const;
    SeenHealthInfo GetPySeenHealthInfo(Entity entity) const;
#endif

    // Networked
    F32 m_angle;
    F32 m_distance;

    // Local (client & server)
    std::vector<Entity> m_seenEntities;
};
}

#endif
