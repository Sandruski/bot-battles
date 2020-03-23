#include "EventComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
EventComponent::EventComponent()
    : m_keyboard()
    , m_mouse()
    , m_mousePosition(0, 0)
{
    m_keyboard.fill(KeyState::IDLE);
    m_mouse.fill(KeyState::IDLE);
}
}
