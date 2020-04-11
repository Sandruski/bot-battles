#include "RendererComponent.h"

#include "Game.h"
#include "ResourceManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RendererComponent::RendererComponent()
    : m_shaderResource()
    , m_meshResource()
    , m_backgroundColor(0.0f, 0.0f, 0.0f, 0.0f)
    , m_isVSync(true)
    , m_isDebugDraw(false)
{
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::LoadFromConfig(const rapidjson::Value& value)
{
    assert(value.HasMember("VSync"));
    m_isVSync = value["VSync"].GetBool();
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::SetWireframe(bool wireframe) const
{
    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

//----------------------------------------------------------------------------------------------------
bool RendererComponent::UpdateVSync() const
{
    if (SDL_GL_SetSwapInterval(m_isVSync) == -1) {
        ELOG("VSync could not be updated");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::UpdateBackgroundColor() const
{
    glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
}
}
