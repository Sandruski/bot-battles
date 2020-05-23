#ifndef __LAYERS_H__
#define __LAYERS_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
enum class LayerType : U16 {

    NEAR_PLANE,

    DEBUG,

    PLAYER,
    WEAPON,
    SIGHT,
    OBJECT,
    FLOOR,
    BACKGROUND,

    FAR_PLANE,

    NONE
};
}

#endif
