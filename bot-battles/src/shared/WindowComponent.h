#ifndef __WINDOW_COMPONENT_H__
#define __WINDOW_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct WindowComponent {

    WindowComponent();

    void LoadFromConfig(const rapidjson::Value& value);

    void UpdateResolution();

    SDL_Window* m_window;

    Vec2I m_resolution;
    bool m_isFullscreen;
};
}

#endif
