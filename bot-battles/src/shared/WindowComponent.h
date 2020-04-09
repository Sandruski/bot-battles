#ifndef __WINDOW_COMPONENT_H__
#define __WINDOW_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct WindowComponent {

    enum DisplayMode : U8 {
        FULLSCREEN,
        WINDOWED,
        BORDERLESS
    };

    WindowComponent();

    void LoadFromConfig(const rapidjson::Value& value);

    void UpdateResolution();
    void UpdateDisplayMode();

    SDL_Window* m_window;

    glm::uvec2 m_resolution;
    bool m_isResizable;

    DisplayMode m_displayMode;
};
}

#endif
