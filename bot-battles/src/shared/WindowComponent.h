#ifndef __WINDOW_COMPONENT_H__
#define __WINDOW_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct WindowComponent : public Subject {

    enum Resolution : U8 {
        LOW,
        MEDIUM,
        HIGH
    };
    enum DisplayMode : U8 {
        FULLSCREEN,
        WINDOWED,
        BORDERLESS
    };

    WindowComponent();

    bool PreUpdate();

    void LoadFromConfig(const rapidjson::Value& value);

    void UpdateCurrentResolution();
    void UpdateResolution();
    bool UpdateDisplayMode();

    void UpdateIcon();

    glm::vec2 GetProportion() const;

    SDL_Window* m_window;
    SDL_Surface* m_iconSurface;

    Resolution m_resolution;
    glm::uvec2 m_currentResolution;
    glm::uvec2 m_baseResolution;
    DisplayMode m_displayMode;
    F32 m_fps;
    bool m_isCap;
    bool m_isResized;
};
}

#endif
