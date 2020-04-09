#ifndef __PANEL_TYPES_H__
#define __PANEL_TYPES_H__

namespace sand {

enum class SettingsPanelType : U8 {

    DISPLAY,

    COUNT
};

enum class DebugOptionsPanelType : U8 {

    GAME,
    RENDERER,
    STATES,

    COUNT
};
}

#endif
