#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WindowComponent::WindowComponent()
    : m_window(nullptr)
    , m_iconSurface(nullptr)
    , m_resolution(Resolution::LOW)
    , m_currentResolution(0, 0)
    , m_baseResolution(896, 504)
    , m_displayMode(DisplayMode::WINDOWED)
    , m_fps(0.0f)
    , m_isCap(true)
    , m_isResized(false)
{
}

//----------------------------------------------------------------------------------------------------
bool WindowComponent::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
void WindowComponent::LoadFromConfig(const rapidjson::Value& value)
{
    if (value.HasMember("resolution") && value["resolution"].IsString()) {
        std::string resolution = value["resolution"].GetString();
        if (resolution == "low") {
            m_resolution = Resolution::LOW;
        } else if (resolution == "medium") {
            m_resolution = Resolution::MEDIUM;
        } else if (resolution == "high") {
            m_resolution = Resolution::HIGH;
        }

        UpdateCurrentResolution();
    }

    if (value.HasMember("display") && value["display"].IsString()) {
        std::string displayMode = value["display"].GetString();
        if (displayMode == "fullscreen") {
            m_displayMode = DisplayMode::FULLSCREEN;
        } else if (displayMode == "windowed") {
            m_displayMode = DisplayMode::WINDOWED;
        } else if (displayMode == "borderless") {
            m_displayMode = DisplayMode::BORDERLESS;
        }
    }

    if (value.HasMember("fps") && value["fps"].IsFloat()) {
        m_fps = value["fps"].GetFloat();
    }
}

//----------------------------------------------------------------------------------------------------
void WindowComponent::UpdateCurrentResolution()
{
    switch (m_resolution) {

    case Resolution::LOW: {
        m_currentResolution = glm::vec2(896, 504);

        m_isResized = true;
        Event newEvent;
        newEvent.eventType = EventType::WINDOW_RESIZED;
        NotifyEvent(newEvent);
        break;
    }

    case Resolution::MEDIUM: {
        m_currentResolution = glm::vec2(1280, 720);

        m_isResized = true;
        Event newEvent;
        newEvent.eventType = EventType::WINDOW_RESIZED;
        NotifyEvent(newEvent);
        break;
    }

    case Resolution::HIGH: {
        m_currentResolution = glm::vec2(1920, 1080);

        m_isResized = true;
        Event newEvent;
        newEvent.eventType = EventType::WINDOW_RESIZED;
        NotifyEvent(newEvent);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void WindowComponent::UpdateResolution()
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

        m_isResized = true;
        Event newEvent;
        newEvent.eventType = EventType::WINDOW_RESIZED;
        NotifyEvent(newEvent);
        break;
    }

    case DisplayMode::WINDOWED: {
        if (SDL_SetWindowFullscreen(m_window, 0) == -1) {
            ELOG("Windowed could not be set");
            return false;
        }
        SDL_SetWindowBordered(m_window, SDL_TRUE);

        UpdateCurrentResolution();
        break;
    }

    case DisplayMode::BORDERLESS: {
        if (SDL_SetWindowFullscreen(m_window, 0) == -1) {
            ELOG("Borderless could not be set");
            return false;
        }
        SDL_SetWindowBordered(m_window, SDL_FALSE);

        UpdateCurrentResolution();
        break;
    }

    default: {
        break;
    }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void WindowComponent::UpdateIcon()
{
    SDL_SetWindowIcon(m_window, m_iconSurface);
}

//----------------------------------------------------------------------------------------------------
glm::vec2 WindowComponent::GetProportion() const
{
    return static_cast<glm::vec2>(m_currentResolution) / static_cast<glm::vec2>(m_baseResolution);
}
}
