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
    assert(value.HasMember("backgroundColor"));
    assert(value["backgroundColor"].IsArray());
    assert(!value["backgroundColor"].Empty());
    assert(value["backgroundColor"][0].IsFloat());
    m_backgroundColor.r = value["backgroundColor"][0].GetFloat();
    assert(value["backgroundColor"][1].IsFloat());
    m_backgroundColor.g = value["backgroundColor"][1].GetFloat();
    assert(value["backgroundColor"][2].IsFloat());
    m_backgroundColor.b = value["backgroundColor"][2].GetFloat();
    assert(value["backgroundColor"][3].IsFloat());
    m_backgroundColor.a = value["backgroundColor"][3].GetFloat();

    // TODO
    /*
    assert(value.HasMember("vsync"));
    assert(value["vsync"].IsBool());
    m_isVsync = value["vsync"].GetBool();
    */

    assert(value.HasMember("debugDraw"));
    assert(value["debugDraw"].IsBool());
    m_isDebugDraw = value["debugDraw"].GetBool();
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
