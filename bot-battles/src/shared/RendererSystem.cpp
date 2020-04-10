#include "RendererSystem.h"

#include "Colors.h"
#include "ComponentManager.h"
#include "DebugDrawer.h"
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

    windowComponent.UpdateResolution();
    if (!windowComponent.UpdateDisplayMode()) {
        return false;
    }

    rendererComponent.UpdateBackgroundColor();
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_CLAMP);

    rendererComponent.m_shaderResource = g_game->GetResourceManager().AddResource<ShaderResource>("", "", false);
    rendererComponent.m_shaderResource.lock()->ForceLoad(defaultVertexShaderSource, defaultFragmentShaderSource);
    glUseProgram(rendererComponent.m_shaderResource.lock()->GetProgram());

    rendererComponent.m_meshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    const std::array<MeshResource::Vertex, 4> vertices = MeshResource::GetQuadVertices();
    const std::array<U32, 6> indices = MeshResource::GetQuadIndices();
    rendererComponent.m_meshResource.lock()->ForceLoad(vertices, indices);

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
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    glm::uvec2 resolution = windowComponent.GetResolution();

    for (auto& entity : m_entities) {

        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !spriteComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::uvec4 textureCoords = spriteComponent.lock()->GetSpriteTextureCoords();

        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 position = transformComponent.lock()->GetPosition();
        position.x += static_cast<F32>(resolution.x) / 2.0f;
        position.y += static_cast<F32>(resolution.y) / 2.0f;
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(transformComponent.lock()->m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(static_cast<F32>(textureCoords.z), static_cast<F32>(textureCoords.w), 0.0f));

        glm::uvec2 size = spriteComponent.lock()->m_spriteResource.lock()->GetSize();
        std::array<MeshResource::Vertex, 4> vertices = MeshResource::GetQuadVertices();
        // Top-left
        vertices[0].m_textureCoords = glm::vec2(textureCoords.x / static_cast<F32>(size.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(size.y));
        // Top-right
        vertices[1].m_textureCoords = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(size.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(size.y));
        // Bottom-left
        vertices[2].m_textureCoords = glm::vec2(textureCoords.x / static_cast<F32>(size.x), 1.0f - textureCoords.y / static_cast<F32>(size.y));
        // Bottom-right
        vertices[3].m_textureCoords = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(size.x), 1.0f - textureCoords.y / static_cast<F32>(size.y));
        rendererComponent.m_meshResource.lock()->ReLoad(vertices);

        U32 modelLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(resolution.x), static_cast<F32>(resolution.y), 0.0f, -static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
        U32 projectionLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        U32 pctLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "pct");
        glUniform1f(pctLoc, 0.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, spriteComponent.lock()->m_spriteResource.lock()->GetTexture());

        glBindVertexArray(rendererComponent.m_meshResource.lock()->GetVAO());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
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
