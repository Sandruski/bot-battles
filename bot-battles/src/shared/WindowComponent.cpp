#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WindowComponent::WindowComponent()
    : m_window(nullptr)
    , m_resolution(0, 0)
    , m_isFullscreen(false)
{
}

//----------------------------------------------------------------------------------------------------
void WindowComponent::LoadFromConfig(const rapidjson::Value& value)
{
    assert(value.HasMember("resolution"));
    assert(value["resolution"].IsArray());
    assert(!value["resolution"].Empty());
    assert(value["resolution"][0].IsUint());
    m_resolution.x = value["resolution"][0].GetUint();
    assert(value["resolution"][1].IsUint());
    m_resolution.y = value["resolution"][1].GetUint();

    assert(value.HasMember("fullscreen"));
    assert(value["fullscreen"].IsBool());
    m_isFullscreen = value["fullscreen"].GetBool();
}

//----------------------------------------------------------------------------------------------------
void WindowComponent::UpdateResolution()
{
    glViewport(0, 0, m_resolution.x, m_resolution.y);
}

//----------------------------------------------------------------------------------------------------
void WindowComponent::UpdateFullscreen()
{
    SDL_SetWindowFullscreen(m_window, m_isFullscreen);
}
}
