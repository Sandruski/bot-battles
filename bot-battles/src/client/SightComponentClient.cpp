#include "SightComponent.h"

#include "BotComponent.h"
#include "ClientComponent.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "GameClient.h"
#include "HealthComponent.h"
#include "RigidbodyComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SightComponent::SeenBotInfo::SeenBotInfo()
    : m_transformComponent()
    , m_colliderComponent()
    , m_rigidbodyComponent()
    , m_weaponComponent()
    , m_healthComponent()
    , m_botComponent()
{
}

//----------------------------------------------------------------------------------------------------
SightComponent::SeenWeaponInfo::SeenWeaponInfo()
    : m_transformComponent()
    , m_colliderComponent()
    , m_weaponComponent()
{
}

//----------------------------------------------------------------------------------------------------
SightComponent::SeenHealthInfo::SeenHealthInfo()
    : m_transformComponent()
    , m_colliderComponent()
    , m_healthComponent()
{
}

//----------------------------------------------------------------------------------------------------
std::vector<Entity> SightComponent::GetPySeenBotEntities() const
{
    std::vector<Entity> seenBots;

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    for (const auto& seenEntity : m_seenEntities) {
        const bool isLocalEntity = clientComponent.lock()->IsLocalEntity(seenEntity);
        if (isLocalEntity) {
            continue;
        }

        std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(seenEntity);
        if (!botComponent.expired()) {
            seenBots.emplace_back(seenEntity);
        }
    }

    return seenBots;
}

//----------------------------------------------------------------------------------------------------
SightComponent::SeenBotInfo SightComponent::GetPySeenBotInfo(Entity entity) const
{
    SightComponent::SeenBotInfo seenBotInfo;

    std::vector<Entity> seenBots = GetPySeenBotEntities();
    std::vector<Entity>::const_iterator it = std::find(seenBots.begin(), seenBots.end(), entity);
    if (it != seenBots.end()) {
        seenBotInfo.m_transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity).lock();
        seenBotInfo.m_colliderComponent = g_gameClient->GetComponentManager().GetComponent<ColliderComponent>(entity).lock();
        seenBotInfo.m_rigidbodyComponent = g_gameClient->GetComponentManager().GetComponent<RigidbodyComponent>(entity).lock();
        seenBotInfo.m_weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity).lock();
        seenBotInfo.m_healthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(entity).lock();
        seenBotInfo.m_botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(entity).lock();
    }

    return seenBotInfo;
}

//----------------------------------------------------------------------------------------------------
std::vector<Entity> SightComponent::GetPySeenWeaponEntities() const
{
    std::vector<Entity> seenWeapons;

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    for (const auto& seenEntity : m_seenEntities) {
        std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(seenEntity);
        if (!botComponent.expired()) {
            continue;
        }

        std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(seenEntity);
        if (!weaponComponent.expired()) {
            seenWeapons.emplace_back(seenEntity);
        }
    }

    return seenWeapons;
}

//----------------------------------------------------------------------------------------------------
SightComponent::SeenWeaponInfo SightComponent::GetPySeenWeaponInfo(Entity entity) const
{
    SightComponent::SeenWeaponInfo seenWeaponInfo;

    std::vector<Entity> seenWeapons = GetPySeenWeaponEntities();
    std::vector<Entity>::const_iterator it = std::find(seenWeapons.begin(), seenWeapons.end(), entity);
    if (it != seenWeapons.end()) {
        seenWeaponInfo.m_transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity).lock();
        seenWeaponInfo.m_colliderComponent = g_gameClient->GetComponentManager().GetComponent<ColliderComponent>(entity).lock();
        seenWeaponInfo.m_weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity).lock();
    }

    return seenWeaponInfo;
}

//----------------------------------------------------------------------------------------------------
std::vector<Entity> SightComponent::GetPySeenHealthEntities() const
{
    std::vector<Entity> seenHealths;

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    for (const auto& seenEntity : m_seenEntities) {
        std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(seenEntity);
        if (!botComponent.expired()) {
            continue;
        }

        std::weak_ptr<HealthComponent> healthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(seenEntity);
        if (!healthComponent.expired()) {
            seenHealths.emplace_back(seenEntity);
        }
    }

    return seenHealths;
}

//----------------------------------------------------------------------------------------------------
SightComponent::SeenHealthInfo SightComponent::GetPySeenHealthInfo(Entity entity) const
{
    SightComponent::SeenHealthInfo seenHealthInfo;

    std::vector<Entity> seenHealths = GetPySeenHealthEntities();
    std::vector<Entity>::const_iterator it = std::find(seenHealths.begin(), seenHealths.end(), entity);
    if (it != seenHealths.end()) {
        seenHealthInfo.m_transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity).lock();
        seenHealthInfo.m_colliderComponent = g_gameClient->GetComponentManager().GetComponent<ColliderComponent>(entity).lock();
        seenHealthInfo.m_healthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(entity).lock();
    }

    return seenHealthInfo;
}
}
