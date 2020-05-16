#ifndef __GAMEPLAY_COMPONENT_H__
#define __GAMEPLAY_COMPONENT_H__

#include "FSM.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct GameplayComponent {

    GameplayComponent();

#ifdef _CLIENT
    void AddLog(const char* log);
    void ClearLogs();
#endif

    FSM m_fsm;
#ifdef _CLIENT
    MyTimer m_mainMenuTimer;
    F32 m_mainMenuTimeout;
    ImGuiTextBuffer m_buf;
    ImVector<I32> m_lineOffsets;
    bool m_isLog;
#endif
};
}

#endif
