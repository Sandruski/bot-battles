#ifndef __COMPONENT_MEMBER_TYPES_H__
#define __COMPONENT_MEMBER_TYPES_H__

namespace sand {

enum class InputComponentMemberType : U32 {

    INPUT_LINEAR_VELOCITY = 1 << 0,
    INPUT_ANGULAR_VELOCITY = 1 << 1,
    INPUT_SHOOT = 1 << 2,
    INPUT_PICK_UP_AMMO = 1 << 3,

    COUNT,

    INPUT_ALL = INPUT_LINEAR_VELOCITY | INPUT_ANGULAR_VELOCITY | INPUT_SHOOT | INPUT_PICK_UP_AMMO
};

enum class ComponentMemberType : U32 {

    // ENGINE
    // Transform
    TRANSFORM_ENABLED = 1 << 0,
    TRANSFORM_POSITION = 1 << 1,
    TRANSFORM_LAYER_TYPE = 1 << 2,
    TRANSFORM_ROTATION = 1 << 3,

    // Collider
    COLLIDER_ENABLED = 1 << 4,
    COLLIDER_SIZE = 1 << 5,
    COLLIDER_SHAPE_TYPE = 1 << 6,

    // Rigidbody
    RIGIDBODY_ENABLED = 1 < 7,
    RIGIDBODY_BODY_TYPE = 1 << 8,
    RIGIDBODY_GROUP_INDEX = 1 << 9,
    RIGIDBODY_BULLET = 1 << 10,

#ifdef _DRAW
    // Label
    LABEL_ENABLED = 1 << 11,
    LABEL_OFFSET = 1 << 12,
    LABEL_TEXT = 1 << 13,
    LABEL_COLOR = 1 << 14,

    // Sprite
    SPRITE_ENABLED = 1 << 15,
    SPRITE_FILE = 1 << 16,
    SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS = 1 << 17,
    SPRITE_SPRITE_NAME = 1 << 18,
#endif

    // GAMEPLAY
    // Weapon
    WEAPON_ENABLED = 1 << 19,
    WEAPON_DAMAGE = 1 << 20,
    WEAPON_AMMO = 1 << 21,
    WEAPON_ORIGIN = 1 << 22,
    WEAPON_DESTINATION = 1 << 23,
    WEAPON_HIT = 1 << 24,

    // Health
    HEALTH_ENABLED = 1 << 25,
    HEALTH_HEALTH = 1 << 26,

    // Sight
    SIGHT_ENABLED = 1 << 27,
    SIGHT_ANGLE = 1 << 28,
    SIGHT_DISTANCE = 1 << 29,

    COUNT,

    TRANSFORM_ALL = TRANSFORM_ENABLED | TRANSFORM_POSITION | TRANSFORM_LAYER_TYPE | TRANSFORM_ROTATION,
    WEAPON_ALL = WEAPON_ENABLED | WEAPON_DAMAGE | WEAPON_AMMO | WEAPON_ORIGIN | WEAPON_DESTINATION | WEAPON_HIT,
    COLLIDER_ALL = COLLIDER_ENABLED | COLLIDER_SIZE | COLLIDER_SHAPE_TYPE,
    RIGIDBODY_ALL = RIGIDBODY_ENABLED | RIGIDBODY_BODY_TYPE | RIGIDBODY_GROUP_INDEX | RIGIDBODY_BULLET,
    HEALTH_ALL = HEALTH_ENABLED | HEALTH_HEALTH,
    SIGHT_ALL = SIGHT_ENABLED | SIGHT_ANGLE | SIGHT_DISTANCE,
#ifdef _DRAW
    LABEL_ALL = LABEL_ENABLED | LABEL_OFFSET | LABEL_TEXT | LABEL_COLOR,
    SPRITE_ALL = SPRITE_ENABLED | SPRITE_FILE | SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS | SPRITE_SPRITE_NAME,

    ALL = TRANSFORM_ALL | WEAPON_ALL | COLLIDER_ALL | RIGIDBODY_ALL | HEALTH_ALL | SIGHT_ALL | LABEL_ALL | SPRITE_ALL
#else

    ALL = TRANSFORM_ALL | WEAPON_ALL | COLLIDER_ALL | RIGIDBODY_ALL | HEALTH_ALL | SIGHT_ALL
#endif
};
}

#endif
