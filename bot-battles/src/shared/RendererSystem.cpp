#include "RendererSystem.h"

#include "Colors.h"
#include "ComponentManager.h"
#include "Game.h"
#include "MeshResource.h"
#include "RendererComponent.h"
#include "ResourceManager.h"
#include "ShaderResource.h"
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

    rendererComponent.m_meshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    const std::vector<MeshResource::Vertex> vertices = MeshResource::GetQuadVertices();
    rendererComponent.m_meshResource.lock()->ForceLoad(vertices);

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

    ILOG("Loop");
    for (std::vector<Entity>::iterator it = m_entities.begin(); it != m_entities.end();) {

        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(*it);
        std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(*it);
        if (!transformComponent.lock()->m_isEnabled || !spriteComponent.lock()->m_isEnabled) {
            continue;
        }

        U32 texture = spriteComponent.lock()->m_spriteResource.lock()->GetTexture();

        std::vector<glm::mat4> models;

        for (std::vector<Entity>::iterator it2 = it; it2 != m_entities.end();) {
            std::weak_ptr<TransformComponent> transformComponent2 = g_game->GetComponentManager().GetComponent<TransformComponent>(*it2);
            std::weak_ptr<SpriteComponent> spriteComponent2 = g_game->GetComponentManager().GetComponent<SpriteComponent>(*it2);
            if (!transformComponent2.lock()->m_isEnabled || !spriteComponent2.lock()->m_isEnabled) {
                continue;
            }

            U32 texture2 = spriteComponent2.lock()->m_spriteResource.lock()->GetTexture();
            if (texture != texture2) {
                break;
            }

            glm::vec3 position = transformComponent2.lock()->GetDebugPositionAndLayer();
            F32 rotation = transformComponent2.lock()->m_rotation;
            glm::uvec4 textureCoords = spriteComponent2.lock()->GetSpriteTextureCoords();
            glm::vec3 scale = glm::vec3(static_cast<F32>(textureCoords.z), static_cast<F32>(textureCoords.w), 0.0f);

            glm::mat4 model = glm::mat4(1.0f);
            position.x *= proportion.x;
            position.y *= proportion.y;
            position.y *= -1.0f;
            model = glm::translate(model, position);
            model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, -1.0f));
            scale.x *= proportion.x;
            scale.y *= proportion.y;
            model = glm::scale(model, scale);

            models.emplace_back(model);

            ++it;
            ++it2;
        }

        //glm::uvec2 textureSize = spriteComponent.lock()->m_spriteResource.lock()->GetSize();
        rendererComponent.DrawTexturedQuad(models, texture);
        ILOG("Draw call");
    }
    ILOG("End loop");

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
}
