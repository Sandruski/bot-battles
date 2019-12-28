#include "ReplicationManagerClient.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "EntityManager.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "MemoryStream.h"
#include "ReplicationCommand.h"
#include "SpriteComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ReplicationManagerClient::ReplicationManagerClient()
{
}

//----------------------------------------------------------------------------------------------------
ReplicationManagerClient::~ReplicationManagerClient()
{
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::Read(InputMemoryStream& inputStream) const
{
    while (inputStream.GetRemainingBitCount() >= 8) {

        NetworkID networkID = INVALID_NETWORK_ID;
        inputStream.Read(networkID);

        ReplicationActionType replicationActionType = ReplicationActionType::INVALID;
        inputStream.Read(replicationActionType, 2);

        switch (replicationActionType) {

        case ReplicationActionType::CREATE: {
            ReadCreateAction(inputStream, networkID);
            break;
        }

        case ReplicationActionType::UPDATE: {
            ReadUpdateAction(inputStream, networkID);
            break;
        }

        case ReplicationActionType::REMOVE: {
            ReadRemoveAction(networkID);
            break;
        }

        default: {
            WLOG("Unknown replication action received from networkID %u", networkID);
            break;
        }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::ReadCreateAction(InputMemoryStream& inputStream, NetworkID networkID) const
{
    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    if (entity == INVALID_ENTITY) {
        entity = g_gameClient->GetEntityManager().AddEntity();
        g_gameClient->GetLinkingContext().AddEntity(entity, networkID);
    }

    ReadUpdateAction(inputStream, networkID);
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::ReadUpdateAction(InputMemoryStream& inputStream, NetworkID networkID) const
{
    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    Signature signature = g_gameClient->GetEntityManager().GetSignature(entity);

    Signature newSignature = 0;
    inputStream.Read(newSignature);
    U32 dirtyState = 0;
    inputStream.Read(dirtyState, GetRequiredBits<static_cast<U32>(ComponentMemberType::COUNT)>::value);

    U16 hasInput = 1 << static_cast<std::size_t>(ComponentType::INPUT);
    const bool hasSignatureInput = signature & hasInput;
    const bool hasNewSignatureInput = newSignature & hasInput;
    if (hasSignatureInput && hasNewSignatureInput) {
        std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetComponentManager().GetComponent<InputComponent>(entity);
        inputComponent.lock()->Read(inputStream, dirtyState);
    } else if (hasSignatureInput) {
        g_gameClient->GetComponentManager().RemoveComponent<InputComponent>(entity);
    } else if (hasNewSignatureInput) {
        std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetComponentManager().AddComponent<InputComponent>(entity);
        inputComponent.lock()->Read(inputStream, dirtyState);
    }

    U16 hasTransform = 1 << static_cast<std::size_t>(ComponentType::TRANSFORM);
    const bool hasSignatureTransform = signature & hasTransform;
    const bool hasNewSignatureTransform = newSignature & hasTransform;
    if (hasSignatureTransform && hasNewSignatureTransform) {
        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        transformComponent.lock()->Read(inputStream, dirtyState);
    } else if (hasSignatureTransform) {
        g_gameClient->GetComponentManager().RemoveComponent<TransformComponent>(entity);
    } else if (hasNewSignatureTransform) {
        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().AddComponent<TransformComponent>(entity);
        transformComponent.lock()->Read(inputStream, dirtyState);
    }

    U16 hasSprite = 1 << static_cast<std::size_t>(ComponentType::SPRITE);
    const bool hasSignatureSprite = signature & hasSprite;
    const bool hasNewSignatureSprite = newSignature & hasSprite;
    if (hasSignatureSprite && hasNewSignatureSprite) {
        std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().GetComponent<SpriteComponent>(entity);
        spriteComponent.lock()->Read(inputStream, dirtyState);
    } else if (hasSignatureSprite) {
        g_gameClient->GetComponentManager().RemoveComponent<SpriteComponent>(entity);
    } else if (hasNewSignatureSprite) {
        std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().AddComponent<SpriteComponent>(entity);
        spriteComponent.lock()->Read(inputStream, dirtyState);
    }

    U16 hasText = 1 << static_cast<std::size_t>(ComponentType::TEXT);
    const bool hasSignatureText = signature & hasText;
    const bool hasNewSignatureText = newSignature & hasText;
    if (hasSignatureText && hasNewSignatureText) {
        std::weak_ptr<TextComponent> textComponent = g_gameClient->GetComponentManager().GetComponent<TextComponent>(entity);
        textComponent.lock()->Read(inputStream, dirtyState);
    } else if (hasSignatureSprite) {
        g_gameClient->GetComponentManager().RemoveComponent<TextComponent>(entity);
    } else if (hasNewSignatureSprite) {
        std::weak_ptr<TextComponent> textComponent = g_gameClient->GetComponentManager().AddComponent<TextComponent>(entity);
        textComponent.lock()->Read(inputStream, dirtyState);
    }

    // TODO: read total size of things written
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::ReadRemoveAction(NetworkID networkID) const
{
    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    if (entity == INVALID_ENTITY) {
        return;
    }

    g_gameClient->GetLinkingContext().RemoveEntity(entity);
    g_gameClient->GetEntityManager().RemoveEntity(entity);
}
}
