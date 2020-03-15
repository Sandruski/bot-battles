#include "RendererSystem.h"

#include "Colors.h"
#include "ComponentManager.h"
#include "DebugDrawer.h"
#include "Game.h"
#include "MeshComponent.h"
#include "RendererComponent.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RendererSystem::RendererSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::SPRITE); // TODO: debug draw should not need having a sprite component!
    m_signature |= 1 << static_cast<U16>(ComponentType::MESH);
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

    SDL_GLContext glContext = SDL_GL_CreateContext(windowComponent.m_window);
    SDL_GL_MakeCurrent(windowComponent.m_window, glContext);
    if (rendererComponent.m_isVsync) {
        if (SDL_GL_SetSwapInterval(1) == -1) {
            ELOG("Vsync could not be set");
            return false;
        }
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
    rendererComponent.UpdateBackgroundColor();

    U32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    I32 success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        ELOG("Shader could not be compiled %s", infoLog);
        return false;
    }

    U32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        ELOG("Shader could not be compiled %s", infoLog);
        return false;
    }

    rendererComponent.m_shader = glCreateProgram();
    glAttachShader(rendererComponent.m_shader, vertexShader);
    glAttachShader(rendererComponent.m_shader, fragmentShader);
    glLinkProgram(rendererComponent.m_shader);
    glGetProgramiv(rendererComponent.m_shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(rendererComponent.m_shader, 512, NULL, infoLog);
        ELOG("Shader could not be linked %s", infoLog);
        return false;
    }

    glUseProgram(rendererComponent.m_shader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::PreRender()
{
    glClear(GL_COLOR_BUFFER_BIT);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::Render()
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    WindowComponent& windowComponent = g_game->GetWindowComponent();

    /*
		1. All level geometry
		2. All debug geometry -> Maybe from each state/system? Render()
		3. The editor -> Maybe from each system? Render()
		4. Swap buffers
	*/

    std::sort(m_entities.begin(), m_entities.end(), [](Entity entity1, Entity entity2) {
        std::weak_ptr<TransformComponent> transformComponent1 = g_game->GetComponentManager().GetComponent<TransformComponent>(entity1);
        std::weak_ptr<TransformComponent> transformComponent2 = g_game->GetComponentManager().GetComponent<TransformComponent>(entity2);
        return transformComponent1.lock()->m_position.z < transformComponent2.lock()->m_position.z;
    });

    for (auto& entity : m_entities) {

        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
        std::weak_ptr<MeshComponent> meshComponent = g_game->GetComponentManager().GetComponent<MeshComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !spriteComponent.lock()->m_isEnabled || !meshComponent.lock()->m_isEnabled) {
            continue;
        }

        if (!spriteComponent.lock()->m_spriteResource.expired()) {
            /*
            const SDL_Rect* srcRect = spriteComponent.lock()->HasCurrentSprite() ? &spriteComponent.lock()->GetCurrentSprite() : nullptr;
            I32 w = spriteComponent.lock()->HasCurrentSprite() ? spriteComponent.lock()->GetCurrentSprite().w : static_cast<I32>(spriteComponent.lock()->m_spriteResource.lock()->GetWidth());
            I32 h = spriteComponent.lock()->HasCurrentSprite() ? spriteComponent.lock()->GetCurrentSprite().h : static_cast<I32>(spriteComponent.lock()->m_spriteResource.lock()->GetHeight());
            I32 x = static_cast<I32>(transformComponent.lock()->m_position.x) - w / 2;
            I32 y = static_cast<I32>(transformComponent.lock()->m_position.y) - h / 2;
            const SDL_Rect dstRect = { x, y, w, h };
            */
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, transformComponent.lock()->m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::translate(model, transformComponent.lock()->m_position);
            glm::uvec2 size = spriteComponent.lock()->m_spriteResource.lock()->GetSize();
            model = glm::scale(model, glm::vec3(static_cast<F32>(size.x), static_cast<F32>(size.y), 0.0f));
            U32 modelLoc = glGetUniformLocation(rendererComponent.m_shader, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glm::mat4 view = glm::mat4(1.0f);
            U32 viewLoc = glGetUniformLocation(rendererComponent.m_shader, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            //F32 aspectRatio = static_cast<F32>(windowComponent.m_resolution.x) / static_cast<F32>(windowComponent.m_resolution.y);
            //glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
            //glm::mat4 projection = glm::ortho(0.0f, -aspectRatio, aspectRatio, 0.0f, -1.0f, 1.0f);
            glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_resolution.x), static_cast<F32>(windowComponent.m_resolution.y), 0.0f, -1.0f, 1.0f);
            U32 projectionLoc = glGetUniformLocation(rendererComponent.m_shader, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

            glBindTexture(GL_TEXTURE_2D, spriteComponent.lock()->m_spriteResource.lock()->GetTexture());
            glBindVertexArray(meshComponent.lock()->m_VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
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
}
