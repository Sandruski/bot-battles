#ifndef __GAMEPLAY_COMPONENT_H__
#define __GAMEPLAY_COMPONENT_H__

namespace sand {

    //----------------------------------------------------------------------------------------------------
    // System Component
    struct GameplayComponent {

        GameplayComponent();

        PhaseType m_phaseType;
    };
}

#endif