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

namespace sand {

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::Read(InputMemoryStream& inputStream) const
{
    U32 frame = 0;
    inputStream.Read(frame);
    ILOG("Frame received %u", frame);

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    if (gameplayComponent.m_phase == GameplayComponent::GameplayPhase::PLAY) {
        F32 startFrameTime = MyTime::GetInstance().GetStartFrameTime();
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        clientComponent.m_frameBuffer.Add(Frame(frame, startFrameTime));
    }

    while (inputStream.GetRemainingBitCount() >= 8) {

        NetworkID networkID = INVALID_NETWORK_ID;
        inputStream.Read(networkID);

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

        if (replicationActionType == ReplicationActionType::UPDATE) {
            LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
            Entity entity = linkingContext.GetEntity(networkID);
            Signature signature = g_gameClient->GetEntityManager().GetSignature(entity);
            const bool hasRemotePlayer = signature & static_cast<U16>(ComponentType::REMOTE_PLAYER);
            if (hasRemotePlayer) {
                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                bool hasTransform = false;
                for (U32 i = transformComponent.lock()->m_transformBuffer.m_front; i < transformComponent.lock()->m_transformBuffer.m_back; ++i) {
                    Transform& t = transformComponent.lock()->m_transformBuffer.Get(i);
                    if (t.GetFrame() == frame) {
                        hasTransform = true;
                        break;
                    }
                }

                if (!hasTransform) {
                    glm::vec3 position = !transformComponent.lock()->m_transformBuffer.IsEmpty() ? transformComponent.lock()->m_transformBuffer.GetLast().m_position : transformComponent.lock()->m_position;
                    F32 rotation = !transformComponent.lock()->m_transformBuffer.IsEmpty() ? transformComponent.lock()->m_transformBuffer.GetLast().m_rotation : transformComponent.lock()->m_rotation;
                    Transform transform = Transform(position, rotation, frame);
                    transformComponent.lock()->m_transformBuffer.Add(transform);
                    ILOG("Added frame %u", frame);
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::ReadCreateAction(InputMemoryStream& inputStream, NetworkID networkID, U32 frame) const
{
    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    if (entity >= INVALID_ENTITY) {
        entity = g_gameClient->GetEntityManager().AddEntity();
        g_gameClient->GetLinkingContext().AddEntity(entity, networkID);
    }

    PlayerID playerID = INVALID_PLAYER_ID;
    inputStream.Read(playerID);
    Signature signature = g_gameClient->GetEntityManager().GetSignature(entity);
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (playerID == clientComponent.m_playerID) {
        clientComponent.m_entity = entity; // TODO: remove
        const bool hasLocalPlayer = signature & static_cast<U16>(ComponentType::LOCAL_PLAYER);
        if (!hasLocalPlayer) {
            g_gameClient->GetComponentManager().AddComponent<LocalPlayerComponent>(entity);
        }
    } else
    // TODO: REMOTE PLAYERS ARE ALSO NETWORKED TILES. CHANGE THIS AND ADD A PLAYER COMPONENT TO THOSE ACTUALLY PLAYERS!
    {
        const bool hasRemotePlayer = signature & static_cast<U16>(ComponentType::REMOTE_PLAYER);
        if (!hasRemotePlayer) {
            g_gameClient->GetComponentManager().AddComponent<RemotePlayerComponent>(entity);
        }
    }

    ReadUpdateAction(inputStream, networkID, frame);
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::ReadUpdateAction(InputMemoryStream& inputStream, NetworkID networkID, U32 frame) const
{
    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    Signature signature = g_gameClient->GetEntityManager().GetSignature(entity);

    Signature newSignature = 0;
    inputStream.Read(newSignature);
    U32 dirtyState = 0;
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

    g_gameClient->GetLinkingContext().RemoveEntity(entity);
    g_gameClient->GetEntityManager().RemoveEntity(entity);
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    clientComponent.m_entity = INVALID_ENTITY;
}
}
