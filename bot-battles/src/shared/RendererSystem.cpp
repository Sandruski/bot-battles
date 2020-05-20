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
    RendererComponent& rendererComponent = g_game->GetRendererComponent();

    for (auto& entity : m_entities) {

        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !spriteComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::vec3 position = transformComponent.lock()->GetDebugPositionAndLayer();
        F32 rotation = transformComponent.lock()->m_rotation;
        glm::uvec4 textureCoords = spriteComponent.lock()->GetSpriteTextureCoords();
        glm::vec3 scale = glm::vec3(static_cast<F32>(textureCoords.z), static_cast<F32>(textureCoords.w), 0.0f);
        U32 texture = spriteComponent.lock()->m_spriteResource.lock()->GetTexture();
        glm::uvec2 textureSize = spriteComponent.lock()->m_spriteResource.lock()->GetSize();
        rendererComponent.DrawTexturedQuad(position, rotation, scale, texture, textureCoords, textureSize);
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
}
