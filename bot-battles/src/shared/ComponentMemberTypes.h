#ifndef __COMPONENT_MEMBER_TYPES_H__
#define __COMPONENT_MEMBER_TYPES_H__

namespace sand {

enum class InputComponentMemberType : U16 {

    INPUT_LINEAR_VELOCITY = 1 << 0,
    INPUT_ANGULAR_VELOCITY = 1 << 1,
    INPUT_SHOOT_PRIMARY_WEAPON = 1 << 2,
    INPUT_SHOOT_SECONDARY_WEAPON = 1 << 3,

    INPUT_ALL = INPUT_LINEAR_VELOCITY | INPUT_ANGULAR_VELOCITY | INPUT_SHOOT_PRIMARY_WEAPON | INPUT_SHOOT_SECONDARY_WEAPON
};

enum class ComponentMemberType : U64 {

    // ENGINE
    // Transform
    TRANSFORM_POSITION = 1ULL << 0,
    TRANSFORM_LAYER_TYPE = 1ULL << 1,
    TRANSFORM_ROTATION = 1ULL << 2,
    TRANSFORM_SCALE = 1ULL << 3,

    // Collider
    COLLIDER_SIZE = 1ULL << 4,
    COLLIDER_SHAPE_TYPE = 1ULL << 5,
    COLLIDER_TRIGGER = 1ULL << 6,

    // Rigidbody
    RIGIDBODY_BODY_TYPE = 1ULL << 7,
    RIGIDBODY_GROUP_INDEX = 1ULL << 8,
    RIGIDBODY_BULLET = 1ULL << 9,

#ifdef _DRAW
    // Label
    LABEL_OFFSET = 1ULL << 10,
    LABEL_TEXT = 1ULL << 11,
    LABEL_COLOR = 1ULL << 12,

    // Sprite
    SPRITE_FILE = 1ULL << 13,
    SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS = 1ULL << 14,
    SPRITE_SPRITE_NAME = 1ULL << 15,
    SPRITE_COLOR = 1ULL << 16,
    SPRITE_PCT = 1ULL << 17,
    SPRITE_VISIBLE = 1ULL << 18,
#endif

    // GAMEPLAY
    // Weapon
    WEAPON_WEAPON_PRIMARY = 1ULL << 19,
    WEAPON_DAMAGE_PRIMARY = 1ULL << 20,
    WEAPON_DAMAGE_SECONDARY = 1ULL << 21,
    WEAPON_AMMO_PRIMARY = 1ULL << 22,
    WEAPON_RANGE_PRIMARY = 1ULL << 23,
    WEAPON_RANGE_SECONDARY = 1ULL << 24,
    WEAPON_COOLDOWN_PRIMARY = 1ULL << 25,
    WEAPON_COOLDOWN_SECONDARY = 1ULL << 26,
    WEAPON_HAS_SHOT = 1ULL << 27,

    // Health
    HEALTH_CURRENT_HEALTH = 1ULL << 28,
    HEALTH_MAX_HEALTH = 1ULL << 29,

    // Sight
    SIGHT_ANGLE = 1ULL << 30,
    SIGHT_DISTANCE = 1ULL << 31,

    TRANSFORM_ALL = TRANSFORM_POSITION | TRANSFORM_LAYER_TYPE | TRANSFORM_ROTATION | TRANSFORM_SCALE,
    WEAPON_ALL = WEAPON_WEAPON_PRIMARY | WEAPON_DAMAGE_PRIMARY | WEAPON_DAMAGE_SECONDARY | WEAPON_AMMO_PRIMARY | WEAPON_RANGE_PRIMARY | WEAPON_RANGE_SECONDARY | WEAPON_COOLDOWN_PRIMARY | WEAPON_COOLDOWN_SECONDARY | WEAPON_HAS_SHOT,
    COLLIDER_ALL = COLLIDER_SIZE | COLLIDER_SHAPE_TYPE | COLLIDER_TRIGGER,
    RIGIDBODY_ALL = RIGIDBODY_BODY_TYPE | RIGIDBODY_GROUP_INDEX | RIGIDBODY_BULLET,
    HEALTH_ALL = HEALTH_CURRENT_HEALTH | HEALTH_MAX_HEALTH,
    SIGHT_ALL = SIGHT_ANGLE | SIGHT_DISTANCE,
#ifdef _DRAW
    LABEL_ALL = LABEL_OFFSET | LABEL_TEXT | LABEL_COLOR,
    SPRITE_ALL = SPRITE_FILE | SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS | SPRITE_SPRITE_NAME | SPRITE_VISIBLE,

    ALL = TRANSFORM_ALL | WEAPON_ALL | COLLIDER_ALL | RIGIDBODY_ALL | HEALTH_ALL | SIGHT_ALL | LABEL_ALL | SPRITE_ALL
#else

    ALL = TRANSFORM_ALL | WEAPON_ALL | COLLIDER_ALL | RIGIDBODY_ALL | HEALTH_ALL | SIGHT_ALL
#endif
};
}

#endif
