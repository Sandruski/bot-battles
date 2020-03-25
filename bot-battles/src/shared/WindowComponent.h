#ifndef __WINDOW_COMPONENT_H__
#define __WINDOW_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct WindowComponent {

    WindowComponent();

    void LoadFromConfig(const rapidjson::Value& value);

    void UpdateResolution();
    void UpdateFullscreen();

    SDL_Window* m_window;

    glm::uvec2 m_resolution;
    bool m_isFullscreen;
};
}

#endif
