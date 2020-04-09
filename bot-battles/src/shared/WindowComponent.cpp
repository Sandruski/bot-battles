#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WindowComponent::WindowComponent()
    : m_window(nullptr)
    , m_resolution(Resolution::LOW)
    , m_displayMode(DisplayMode::WINDOWED)
{
}

//----------------------------------------------------------------------------------------------------
void WindowComponent::LoadFromConfig(const rapidjson::Value& /*value*/)
{
    // TODO

    /*
    assert(value.HasMember("resolution"));
    assert(value["resolution"].IsArray());
    assert(!value["resolution"].Empty());
    assert(value["resolution"][0].IsUint());
    m_resolution.x = value["resolution"][0].GetUint();
    assert(value["resolution"][1].IsUint());
    m_resolution.y = value["resolution"][1].GetUint();
    */

    /*
    assert(value.HasMember("fullscreen"));
    assert(value["fullscreen"].IsBool());
    m_isFullscreen = value["fullscreen"].GetBool();
    */
}

//----------------------------------------------------------------------------------------------------
void WindowComponent::UpdateResolution() const
{
    glm::uvec2 resolution = GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

//----------------------------------------------------------------------------------------------------
bool WindowComponent::UpdateDisplayMode() const
{
    switch (m_displayMode) {

    case DisplayMode::FULLSCREEN: {
        if (SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN) == -1) {
            ELOG("Fullscreen could not be set");
            return false;
        }
        break;
    }

    case DisplayMode::WINDOWED: {
        if (SDL_SetWindowFullscreen(m_window, 0) == -1) {
            ELOG("Windowed could not be set");
            return false;
        }
        SDL_SetWindowBordered(m_window, SDL_TRUE);
        break;
    }

    case DisplayMode::BORDERLESS: {
        if (SDL_SetWindowFullscreen(m_window, 0) == -1) {
            ELOG("Borderless could not be set");
            return false;
        }
        SDL_SetWindowBordered(m_window, SDL_FALSE);
        break;
    }

    default: {
        break;
    }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
glm::uvec2 WindowComponent::GetResolution() const
{
    glm::vec2 resolution = glm::vec2(0, 0);

    switch (m_resolution) {

    case Resolution::MAX: {
        resolution = glm::vec2(1920, 1080);
        break;
    }

    case Resolution::MID: {
        resolution = glm::vec2(1280, 720);
        break;
    }

    case Resolution::LOW: {
        resolution = glm::vec2(640, 480);
        break;
    }

    default: {
        break;
    }
    }

    return resolution;
}
}
