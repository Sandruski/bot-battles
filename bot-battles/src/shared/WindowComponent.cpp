#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WindowComponent::WindowComponent()
    : m_window(nullptr)
    , m_resolution(Resolution::LOW)
    , m_displayMode(DisplayMode::WINDOWED)
    , m_fps(0.0f)
{
}

//----------------------------------------------------------------------------------------------------
void WindowComponent::LoadFromConfig(const rapidjson::Value& value)
{
    assert(value.HasMember("resolution"));
    std::string resolution = value["resolution"].GetString();
    if (resolution == "low") {
        m_resolution = Resolution::LOW;
    } else if (resolution == "medium") {
        m_resolution = Resolution::MEDIUM;
    } else if (resolution == "high") {
        m_resolution = Resolution::HIGH;
    }

    assert(value.HasMember("displayMode"));
    std::string displayMode = value["displayMode"].GetString();
    if (displayMode == "fullscreen") {
        m_displayMode = DisplayMode::FULLSCREEN;
    } else if (displayMode == "windowed") {
        m_displayMode = DisplayMode::WINDOWED;
    } else if (displayMode == "borderless") {
        m_displayMode = DisplayMode::BORDERLESS;
    }

    assert(value.HasMember("fps"));
    m_fps = value["fps"].GetFloat();
}

//----------------------------------------------------------------------------------------------------
void WindowComponent::UpdateResolution() const
{
    glm::uvec2 resolution = GetResolution();
    SDL_SetWindowSize(m_window, resolution.x, resolution.y);
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

    case Resolution::LOW: {
        resolution = glm::vec2(640, 480);
        break;
    }

    case Resolution::MEDIUM: {
        resolution = glm::vec2(1280, 720);
        break;
    }

    case Resolution::HIGH: {
        resolution = glm::vec2(1920, 1080);
        break;
    }

    default: {
        break;
    }
    }

    return resolution;
}
}
