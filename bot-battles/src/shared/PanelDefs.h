#ifndef __PANEL_DEFS_H__
#define __PANEL_DEFS_H__

#include "PanelTypes.h"

namespace sand {

const U8 MAX_SETTINGS_PANELS = static_cast<U8>(SettingsPanelType::COUNT);
const U8 INVALID_SETTINGS_PANEL = MAX_SETTINGS_PANELS;

const U8 MAX_DEBUG_OPTIONS_PANELS = static_cast<U8>(DebugOptionsPanelType::COUNT);
const U8 INVALID_DEBUG_OPTIONS_PANEL = MAX_DEBUG_OPTIONS_PANELS;
}

#endif
