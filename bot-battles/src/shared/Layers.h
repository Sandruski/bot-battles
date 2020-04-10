#ifndef __LAYERS_H__
#define __LAYERS_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
enum class LayerType : U16 {

    NEAR_PLANE = 1,

    DEBUG,

    PLAYER,
    OBJECT,
    FLOOR,
    BACKGROUND,

    FAR_PLANE,

    NONE
};
}

#endif
