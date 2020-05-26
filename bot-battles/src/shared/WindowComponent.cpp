#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WindowComponent::WindowComponent()
    : m_window(nullptr)
    , m_resolution(Resolution::LOW)
    , m_currentResolution(0, 0)
    , m_baseResolution(896, 504)
    , m_displayMode(DisplayMode::WINDOWED)
    , m_fps(0.0f)
    , m_isCap(true)
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

    UpdateCurrentResolution();

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
void WindowComponent::UpdateCurrentResolution()
{
    switch (m_resolution) {

    case Resolution::LOW: {
        m_currentResolution = glm::vec2(896, 504);
        break;
    }

    case Resolution::MEDIUM: {
        m_currentResolution = glm::vec2(1280, 720);
        break;
    }

    case Resolution::HIGH: {
        m_currentResolution = glm::vec2(1920, 1080);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void WindowComponent::UpdateResolution() const
{
    SDL_SetWindowSize(m_window, m_currentResolution.x, m_currentResolution.y);
    glViewport(0, 0, m_currentResolution.x, m_currentResolution.y);
}

//----------------------------------------------------------------------------------------------------
bool WindowComponent::UpdateDisplayMode()
{
    switch (m_displayMode) {

    case DisplayMode::FULLSCREEN: {
        if (SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP) == -1) {
            ELOG("Fullscreen could not be set");
            return false;
        }

        SDL_DisplayMode dm;
        if (SDL_GetDesktopDisplayMode(0, &dm) == -1) {
            ELOG("Fullscreen could not be set");
            return false;
        }

        m_currentResolution = glm::uvec2(dm.w, dm.h);
        UpdateResolution();
        break;
    }

    case DisplayMode::WINDOWED: {
        if (SDL_SetWindowFullscreen(m_window, 0) == -1) {
            ELOG("Windowed could not be set");
            return false;
        }
        SDL_SetWindowBordered(m_window, SDL_TRUE);

        UpdateCurrentResolution();
        UpdateResolution();
        break;
    }

    case DisplayMode::BORDERLESS: {
        if (SDL_SetWindowFullscreen(m_window, 0) == -1) {
            ELOG("Borderless could not be set");
            return false;
        }
        SDL_SetWindowBordered(m_window, SDL_FALSE);

        UpdateCurrentResolution();
        UpdateResolution();
        break;
    }

    default: {
        break;
    }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
glm::vec2 WindowComponent::GetProportion() const
{
    return static_cast<glm::vec2>(m_currentResolution) / static_cast<glm::vec2>(m_baseResolution);
}
}
