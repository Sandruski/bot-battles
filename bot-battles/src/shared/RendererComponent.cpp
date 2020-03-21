#include "RendererComponent.h"

#include "Game.h"
#include "ResourceManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RendererComponent::RendererComponent()
    : m_shaderResource()
    , m_meshResource()
    , m_backgroundColor(0.0f, 0.0f, 0.0f, 0.0f)
    , m_isVsync(false)
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

    assert(value.HasMember("vsync"));
    assert(value["vsync"].IsBool());
    m_isVsync = value["vsync"].GetBool();

    assert(value.HasMember("debugDraw"));
    assert(value["debugDraw"].IsBool());
    m_isDebugDraw = value["debugDraw"].GetBool();
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::UpdateBackgroundColor()
{
    glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
}
}
