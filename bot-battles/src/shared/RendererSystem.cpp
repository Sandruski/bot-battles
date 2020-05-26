#include "RendererSystem.h"

#include "Colors.h"
#include "ComponentManager.h"
#include "Game.h"
#include "MeshResource.h"
#include "RendererComponent.h"
#include "ResourceManager.h"
#include "ShaderResource.h"
#include "SightComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RendererSystem::RendererSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::SPRITE); // TODO: debug draw should not require having a sprite component!
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::StartUp()
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    WindowComponent& windowComponent = g_game->GetWindowComponent();

    // GL 3.0 + GLSL 130
    //const char* glslVersion = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GLContext glContext = SDL_GL_CreateContext(windowComponent.m_window);
    SDL_GL_MakeCurrent(windowComponent.m_window, glContext);
    if (!rendererComponent.UpdateVSync()) {
        return false;
    }

    if (gl3wInit()) {
        ELOG("OpenGL could not be initialized");
        return false;
    }
    if (!gl3wIsSupported(3, 0)) {
        ELOG("OpenGL 3.0 is not supported");
        return false;
    }

    if (!windowComponent.UpdateDisplayMode()) {
        return false;
    }
    windowComponent.UpdateResolution();

    rendererComponent.UpdateBackgroundColor();
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    rendererComponent.m_shaderResource = g_game->GetResourceManager().AddResource<ShaderResource>("", "", false);
    rendererComponent.m_shaderResource.lock()->ForceLoad(defaultVertexShaderSource, defaultFragmentShaderSource);
    glUseProgram(rendererComponent.m_shaderResource.lock()->GetProgram());

    rendererComponent.m_lineMeshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    rendererComponent.m_lineMeshResource.lock()->ForceLoad();
    rendererComponent.m_circleMeshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    rendererComponent.m_circleMeshResource.lock()->ForceLoad();
    rendererComponent.m_quadMeshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    rendererComponent.m_quadMeshResource.lock()->ForceLoad();
    const std::vector<MeshResource::Vertex> quadVertices = MeshResource::GetQuadVertices();
    rendererComponent.m_quadMeshResource.lock()->ReLoadVertices(quadVertices);

    rendererComponent.m_mapMeshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    rendererComponent.m_mapMeshResource.lock()->ForceLoad();
    rendererComponent.m_mapMeshResource.lock()->ReLoadVertices(quadVertices);
    rendererComponent.m_charactersMeshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    rendererComponent.m_charactersMeshResource.lock()->ForceLoad();
    rendererComponent.m_charactersMeshResource.lock()->ReLoadVertices(quadVertices);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::PreRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::Render()
{
    OPTICK_EVENT();

    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

    std::sort(m_entities.begin(), m_entities.end(),
        [](const Entity& entityA, const Entity& entityB) -> bool {
            std::weak_ptr<SpriteComponent> spriteComponentA = g_game->GetComponentManager().GetComponent<SpriteComponent>(entityA);
            std::weak_ptr<SpriteComponent> spriteComponentB = g_game->GetComponentManager().GetComponent<SpriteComponent>(entityB);
            return spriteComponentA.lock()->m_spriteResource.lock()->GetTexture() < spriteComponentB.lock()->m_spriteResource.lock()->GetTexture();
        });

    for (U32 i = 0; i < m_entities.size();) {
        Entity entity = m_entities.at(i);
        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
        if (!spriteComponent.lock()->m_isVisible) {
            ++i;
            continue;
        }

        U32 texture = spriteComponent.lock()->m_spriteResource.lock()->GetTexture();

        for (; i < m_entities.size();) {
            Entity entity2 = m_entities.at(i);
            std::weak_ptr<TransformComponent> transformComponent2 = g_game->GetComponentManager().GetComponent<TransformComponent>(entity2);
            std::weak_ptr<SpriteComponent> spriteComponent2 = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity2);
            if (!spriteComponent2.lock()->m_isVisible) {
                ++i;
                continue;
            }

            U32 texture2 = spriteComponent2.lock()->m_spriteResource.lock()->GetTexture();
            if (texture != texture2) {
                break;
            }

            ++i;
        }

        const std::string textureFile = spriteComponent.lock()->m_spriteResource.lock()->GetFile();
        if (textureFile == "map.png") {
            rendererComponent.DrawMapTexturedQuad(i, texture);
        } else if (textureFile == "characters.png") {
            rendererComponent.DrawCharactersTexturedQuad(i, texture);
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::PostRender()
{
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    SDL_GL_SwapWindow(windowComponent.m_window);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::ShutDown()
{
    SDL_GL_DeleteContext(SDL_GL_GetCurrentContext());

    return true;
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::SYSTEM_ENTITY_ADDED: {
        if (event.system.systemType == GetType()) {
            OnSystemEntityAdded(event.system.entity);
        }
        break;
    }

    case EventType::SYSTEM_ENTITY_REMOVED: {
        if (event.system.systemType == GetType()) {
            OnSystemEntityRemoved(event.system.entity);
        }
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::OnSystemEntityAdded(Entity /*entity*/) const
{
    RecalculateMesh();
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::OnSystemEntityRemoved(Entity /*entity*/) const
{
    RecalculateMesh();
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::RecalculateMesh() const
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

    std::vector<MeshResource::Instance> mapInstances;
    std::vector<MeshResource::Instance> charactersInstances;
    for (const auto& entity : m_entities) {
        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
        if (!spriteComponent.lock()->m_isVisible || spriteComponent.lock()->m_spriteResource.expired()) {
            continue;
        }

        MeshResource::Instance instance;

        glm::vec3 position = transformComponent.lock()->GetPositionAndLayer();
        F32 rotation = transformComponent.lock()->m_rotation;
        glm::uvec4 textureCoords = spriteComponent.lock()->GetSpriteTextureCoords();
        glm::vec3 scale = glm::vec3(static_cast<F32>(textureCoords.z), static_cast<F32>(textureCoords.w), 0.0f);
        scale *= transformComponent.lock()->m_scale;

        glm::mat4 model = glm::mat4(1.0f);
        position.x *= proportion.x;
        position.y *= proportion.y;
        position.y *= -1.0f;
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, -1.0f));
        scale.x *= proportion.x;
        scale.y *= proportion.y;
        model = glm::scale(model, scale);
        instance.m_model = model;

        glm::uvec2 textureSize = spriteComponent.lock()->m_spriteResource.lock()->GetSize();
        glm::vec2 texCoords0 = glm::vec2(textureCoords.x / static_cast<F32>(textureSize.x), 1.0f - textureCoords.y / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[0] = texCoords0;
        glm::vec2 texCoords1 = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(textureSize.x), 1.0f - textureCoords.y / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[1] = texCoords1;
        glm::vec2 texCoords2 = glm::vec2(textureCoords.x / static_cast<F32>(textureSize.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[2] = texCoords2;
        glm::vec2 texCoords3 = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(textureSize.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[3] = texCoords3;

        const std::string textureFile = spriteComponent.lock()->m_spriteResource.lock()->GetFile();
        if (textureFile == "map.png") {
            mapInstances.emplace_back(instance);
        } else if (textureFile == "characters.png") {
            charactersInstances.emplace_back(instance);
        }
    }

    rendererComponent.m_mapMeshResource.lock()->ReLoadInstances(mapInstances);
    rendererComponent.m_charactersMeshResource.lock()->ReLoadInstances(charactersInstances);
}
}
