#include "RendererSystem.h"

#include "ComponentManager.h"
#include "DebugDrawer.h"
#include "FontResource.h"
#include "Game.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"

#include "RendererComponent.h"
#include "WindowComponent.h"

#include "Colors.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RendererSystem::RendererSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::SPRITE); // TODO: debug draw should not need having a sprite component!
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::StartUp()
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();

    // GL 3.0 + GLSL 130
    //const char* glslVersion = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    WindowComponent& windowComponent = g_game->GetWindowComponent();
    SDL_GLContext glContext = SDL_GL_CreateContext(windowComponent.m_window);
    SDL_GL_MakeCurrent(windowComponent.m_window, glContext);
    if (rendererComponent.m_isVsync) {
        SDL_GL_SetSwapInterval(1);
    }

    if (gl3wInit()) {
        ELOG("OpenGL could not be initialized");
        return false;
    }
    if (!gl3wIsSupported(3, 0)) {
        ELOG("OpenGL 3.0 is not supported");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::PreRender()
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();

    //SDL_SetRenderDrawColor(rendererComponent.m_renderer, rendererComponent.m_backgroundColor.r, rendererComponent.m_backgroundColor.g, rendererComponent.m_backgroundColor.b, rendererComponent.m_backgroundColor.a);
    //SDL_RenderClear(rendererComponent.m_renderer);

    //glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(rendererComponent.m_backgroundColor.r, rendererComponent.m_backgroundColor.g, rendererComponent.m_backgroundColor.b, rendererComponent.m_backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::Render()
{
    return true;

    //RendererComponent& rendererComponent = g_game->GetRendererComponent();

    /*
		1. All level geometry
		2. All debug geometry -> Maybe from each state/system? Render()
		3. The editor -> Maybe from each system? Render()
		4. Swap buffers
	*/

    /*
    std::sort(m_entities.begin(), m_entities.end(), [](Entity entity1, Entity entity2) {
        std::weak_ptr<TransformComponent> transformComponent1 = g_game->GetComponentManager().GetComponent<TransformComponent>(entity1);
        std::weak_ptr<TransformComponent> transformComponent2 = g_game->GetComponentManager().GetComponent<TransformComponent>(entity2);
        return transformComponent1.lock()->m_position.z < transformComponent2.lock()->m_position.z;
    });

    for (auto& entity : m_entities) {

        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !spriteComponent.lock()->m_isEnabled) {
            continue;
        }

        if (!spriteComponent.lock()->m_spriteResource.expired()) {
            const SDL_Rect* srcRect = spriteComponent.lock()->HasCurrentSprite() ? &spriteComponent.lock()->GetCurrentSprite() : nullptr;
            I32 w = spriteComponent.lock()->HasCurrentSprite() ? spriteComponent.lock()->GetCurrentSprite().w : static_cast<I32>(spriteComponent.lock()->m_spriteResource.lock()->GetWidth());
            I32 h = spriteComponent.lock()->HasCurrentSprite() ? spriteComponent.lock()->GetCurrentSprite().h : static_cast<I32>(spriteComponent.lock()->m_spriteResource.lock()->GetHeight());
            I32 x = static_cast<I32>(transformComponent.lock()->m_position.x) - w / 2;
            I32 y = static_cast<I32>(transformComponent.lock()->m_position.y) - h / 2;
            const SDL_Rect dstRect = { x, y, w, h };
            SDL_RenderCopyEx(rendererComponent.m_renderer,
                spriteComponent.lock()->m_spriteResource.lock()->GetTexture(),
                srcRect,
                &dstRect,
                transformComponent.lock()->m_rotation,
                nullptr,
                SDL_FLIP_NONE);
        }

        if (rendererComponent.m_isDebugDraw) {

            DebugDrawer::DrawQuad(
                {
                    (int)transform->m_position.x,
                    (int)transform->m_position.y,
                    (int)transform->m_position.x + 10,
                    (int)transform->m_position.y + 10,
                },
                Red);

			DebugDrawer::DrawQuad(
				{
				(int)g_engine->GetWindow().GetWidth() / 4,
				(int)g_engine->GetWindow().GetHeight() / 4,
				(int)g_engine->GetWindow().GetWidth() / 2,
				(int)g_engine->GetWindow().GetHeight() / 2,
				},
				Red);

			DebugDrawer::DrawCircle(
				(int)g_engine->GetWindow().GetWidth() / 4,
				(int)g_engine->GetWindow().GetHeight() / 4,
				50,
				Green);

			DebugDrawer::DrawLine(
				{
				0,
				(int)g_engine->GetWindow().GetHeight() / 2,
				(int)g_engine->GetWindow().GetWidth(),
				(int)g_engine->GetWindow().GetHeight() / 2,
				},
				Blue);
        }
    }


    return true;    */
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::PostRender()
{
    //RendererComponent& rendererComponent = g_game->GetRendererComponent();

    //SDL_RenderPresent(rendererComponent.m_renderer);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::ShutDown()
{
    SDL_GL_DeleteContext(SDL_GL_GetCurrentContext());

    return true;
}
}
