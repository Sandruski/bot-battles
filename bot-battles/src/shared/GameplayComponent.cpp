#include "GameplayComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameplayComponent::GameplayComponent()
    : m_fsm()
#ifdef _CLIENT
    , m_mainMenuTimer()
    , m_mainMenuTimeout(60.0f)
    , m_buf()
    , m_lineOffsets()
    , m_isLog(true)
#endif
{
#ifdef _CLIENT
    m_lineOffsets.push_back(0);
#endif
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void GameplayComponent::AddLog(const char* log)
{
    I32 oldSize = m_buf.size();
    m_buf.appendf(log);
    for (int newSize = m_buf.size(); oldSize < newSize; ++oldSize) {
        if (m_buf[oldSize] == '\n') {
            m_lineOffsets.push_back(oldSize + 1);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void GameplayComponent::ClearLogs()
{
    m_buf.clear();
    m_lineOffsets.clear();
    m_lineOffsets.push_back(0);
}
#endif
}
