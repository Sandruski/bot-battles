#include "ReplicationManagerClient.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "EntityManager.h"
#include "Frame.h"
#include "GameClient.h"
#include "LinkingContext.h"
#include "LocalPlayerComponent.h"
#include "NetworkableReadObject.h"
#include "RemotePlayerComponent.h"
#include "ReplicationCommand.h"
#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::Read(InputMemoryStream& inputStream)
{
    U32 frame = 0;
    inputStream.Read(frame);
    ILOG("Frame received %u", frame);

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (clientComponent.m_isEntityInterpolation) {
        F32 startFrameTime = MyTime::GetInstance().GetStartFrameTime();
        clientComponent.m_frameBuffer.Add(Frame(frame, startFrameTime));
    }

    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();

    while (inputStream.GetRemainingBitCount() >= 8) {
        NetworkID networkID = INVALID_NETWORK_ID;
        inputStream.Read(networkID);
        bool isReplicated = false;
        inputStream.Read(isReplicated);
        bool wasReplicated = false;
        inputStream.Read(wasReplicated);

        if (isReplicated) {
            ReplicationActionType replicationActionType = ReplicationActionType::NONE;
            inputStream.Read(replicationActionType, 2);

            switch (replicationActionType) {

            case ReplicationActionType::CREATE: {
                ILOG("CREATE RECEIVED");
                ReadCreateAction(inputStream, networkID, frame);
                break;
            }

            case ReplicationActionType::UPDATE: {
                ILOG("UPDATE RECEIVED");
                ReadUpdateAction(inputStream, networkID, frame);
                break;
            }

            case ReplicationActionType::REMOVE: {
                ReadRemoveAction(networkID);
                ILOG("REMOVE RECEIVED");
                break;
            }

            default: {
                WLOG("Unknown replication action received from networkID %u", networkID);
                break;
            }
            }
        }

        if (!isReplicated && wasReplicated) {
            Event newEvent;
            newEvent.eventType = EventType::SEEN_LOST_ENTITY;
            Entity entity = linkingContext.GetEntity(networkID);
            newEvent.sight.entity = entity;
            PushEvent(newEvent);
        } else if (isReplicated && !wasReplicated) {
            Event newEvent;
            newEvent.eventType = EventType::SEEN_NEW_ENTITY;
            Entity entity = linkingContext.GetEntity(networkID);
            newEvent.sight.entity = entity;
            PushEvent(newEvent);
        }
    }

    if (clientComponent.m_isEntityInterpolation) {
        const std::unordered_map<NetworkID, Entity>& networkIDToEntityMap = linkingContext.GetNetworkIDToEntityMap();
        for (const auto& pair : networkIDToEntityMap) {
            Entity entity = pair.second;
            Signature signature = g_gameClient->GetEntityManager().GetSignature(entity);
            const bool hasRemotePlayer = signature & static_cast<U16>(ComponentType::REMOTE_PLAYER);
            if (hasRemotePlayer) {
                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                U32 index = transformComponent.lock()->m_transformBuffer.m_front;
                bool isFound = false;
                while (index < transformComponent.lock()->m_transformBuffer.m_back) {
                    const Transform& transform = transformComponent.lock()->m_transformBuffer.Get(index);
                    if (transform.GetFrame() == frame) {
                        isFound = true;
                        break;
                    }
                    ++index;
                }

                if (!isFound) {
                    glm::vec2 position = !transformComponent.lock()->m_transformBuffer.IsEmpty() ? transformComponent.lock()->m_transformBuffer.GetLast().m_position : transformComponent.lock()->m_position;
                    F32 rotation = !transformComponent.lock()->m_transformBuffer.IsEmpty() ? transformComponent.lock()->m_transformBuffer.GetLast().m_rotation : transformComponent.lock()->m_rotation;
                    Transform transform = Transform(position, rotation, frame);
                    transformComponent.lock()->m_transformBuffer.Add(transform);
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::ReadCreateAction(InputMemoryStream& inputStream, NetworkID networkID, U32 frame) const
{
    PlayerID playerID = INVALID_PLAYER_ID;
    inputStream.Read(playerID);

    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    if (entity >= INVALID_ENTITY) {
        entity = g_gameClient->GetEntityManager().AddEntity();
        g_gameClient->GetLinkingContext().AddEntity(entity, networkID);

        Signature signature = g_gameClient->GetEntityManager().GetSignature(entity);
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        if (playerID == clientComponent.m_playerID) {
            clientComponent.m_entity = entity;
            const bool hasLocalPlayer = signature & static_cast<U16>(ComponentType::LOCAL_PLAYER);
            if (!hasLocalPlayer) {
                g_gameClient->GetComponentManager().AddComponent<LocalPlayerComponent>(entity);
            }
        } else /*if (playerID != INVALID_PLAYER_ID)*/ {
            const bool hasRemotePlayer = signature & static_cast<U16>(ComponentType::REMOTE_PLAYER); // TODO: remote players are bots and ammo
            if (!hasRemotePlayer) {
                g_gameClient->GetComponentManager().AddComponent<RemotePlayerComponent>(entity);
            }
        }
    }

    ReadUpdateAction(inputStream, networkID, frame);
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::ReadUpdateAction(InputMemoryStream& inputStream, NetworkID networkID, U32 frame) const
{
    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    if (entity >= INVALID_ENTITY) {
        return;
    }

    Signature signature = g_gameClient->GetEntityManager().GetSignature(entity);

    Signature newSignature = 0;
    inputStream.Read(newSignature);
    U64 dirtyState = 0;
    inputStream.Read(dirtyState);

    for (U16 i = 0; i < MAX_NETWORK_COMPONENTS; ++i) {

        U16 hasComponent = 1 << i;
        const bool hasSignatureComponent = signature & hasComponent;
        const bool hasNewSignatureComponent = newSignature & hasComponent;
        if (hasSignatureComponent && hasNewSignatureComponent) {
            std::weak_ptr<Component> component = g_gameClient->GetComponentManager().GetComponent(static_cast<ComponentType>(i), entity);
            std::dynamic_pointer_cast<NetworkableReadObject>(component.lock())->Read(inputStream, dirtyState, frame, ReplicationActionType::UPDATE, entity);
        } else if (hasSignatureComponent) {
            g_gameClient->GetComponentManager().RemoveComponent(static_cast<ComponentType>(i), entity);
        } else if (hasNewSignatureComponent) {
            std::weak_ptr<Component> component = g_gameClient->GetComponentManager().AddComponent(static_cast<ComponentType>(i), entity);
            std::dynamic_pointer_cast<NetworkableReadObject>(component.lock())->Read(inputStream, dirtyState, frame, ReplicationActionType::CREATE, entity);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::ReadRemoveAction(NetworkID networkID) const
{
    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    if (entity >= INVALID_ENTITY) {
        return;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (entity == clientComponent.m_entity) {
        clientComponent.m_entity = INVALID_ENTITY;
    }

    g_gameClient->GetLinkingContext().RemoveEntity(entity);
    g_gameClient->GetEntityManager().RemoveEntity(entity);
}
}
