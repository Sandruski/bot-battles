#ifndef __COMPONENT_MEMBER_TYPES_H__
#define __COMPONENT_MEMBER_TYPES_H__

namespace sand {

enum class InputComponentMemberType : U16 {

    INPUT_LINEAR_VELOCITY = 1 << 0,
    INPUT_ANGULAR_VELOCITY = 1 << 1,
    INPUT_SHOOT_PRIMARY_WEAPON = 1 << 2,
    INPUT_SHOOT_SECONDARY_WEAPON = 1 << 3,
    INPUT_RELOAD = 1 << 4,
    INPUT_HEAL = 1 << 5,

    INPUT_ALL = INPUT_LINEAR_VELOCITY | INPUT_ANGULAR_VELOCITY | INPUT_SHOOT_PRIMARY_WEAPON | INPUT_SHOOT_SECONDARY_WEAPON | INPUT_RELOAD | INPUT_HEAL
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
    RIGIDBODY_MAX_LINEAR_VELOCITY = 1ULL << 7,
    RIGIDBODY_MAX_ANGULAR_VELOCITY = 1ULL << 8,
    RIGIDBODY_BODY_TYPE = 1ULL << 9,
    RIGIDBODY_GROUP_INDEX = 1ULL << 10,
    RIGIDBODY_BULLET = 1ULL << 11,

    // Label
    LABEL_OFFSET = 1ULL << 12,
    LABEL_EXTRA_OFFSET = 1ULL << 13,
    LABEL_TEXT = 1ULL << 14,
    LABEL_COLOR = 1ULL << 15,

    // Sprite
    SPRITE_FILE = 1ULL << 16,
    SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS = 1ULL << 17,
    SPRITE_SPRITE_NAME = 1ULL << 18,
    SPRITE_VISIBLE = 1ULL << 19,
    SPRITE_COLOR = 1ULL << 20,
    SPRITE_PCT = 1ULL << 21,

    // GAMEPLAY
    // Weapon
    WEAPON_DAMAGE_PRIMARY = 1ULL << 22,
    WEAPON_DAMAGE_SECONDARY = 1ULL << 23,
    WEAPON_CURRENT_AMMO_PRIMARY = 1ULL << 24,
    WEAPON_MAX_AMMO_PRIMARY = 1ULL << 25,
    WEAPON_AMMO_PRIMARY = 1ULL << 26,
    WEAPON_RANGE_PRIMARY = 1ULL << 27,
    WEAPON_RANGE_SECONDARY = 1ULL << 28,
    WEAPON_TIME_SHOOT_PRIMARY = 1ULL << 29,
    WEAPON_COOLDOWN_SHOOT_PRIMARY = 1ULL << 30,
    WEAPON_TIME_SHOOT_SECONDARY = 1ULL << 31,
    WEAPON_COOLDOWN_SHOOT_SECONDARY = 1ULL << 32,
    WEAPON_TIME_RELOAD = 1ULL << 33,
    WEAPON_COOLDOWN_RELOAD = 1ULL << 34,
    WEAPON_ORIGIN_LAST_SHOT = 1ULL << 35,
    WEAPON_DESTINATION_LAST_SHOT = 1ULL << 36,
    WEAPON_HIT_ENTITY_LAST_SHOT = 1ULL << 37,

    // Health
    HEALTH_CURRENT_HP = 1ULL << 38,
    HEALTH_MAX_HP = 1ULL << 39,
    HEALTH_HP = 1ULL << 40,
    HEALTH_TIME_HEAL = 1ULL << 41,
    HEALTH_COOLDOWN_HEAL = 1ULL << 42,
    HEALTH_HIT_ENTITY_LAST_SHOT = 1ULL << 43,
    HEALTH_DIRECTION_LAST_SHOT = 1ULL << 44,

    // Sight
    SIGHT_ANGLE = 1ULL << 45,
    SIGHT_DISTANCE = 1ULL << 46,

    // Bot
    BOT_ACTION_TYPE = 1ULL << 47,

    TRANSFORM_ALL = TRANSFORM_POSITION | TRANSFORM_LAYER_TYPE | TRANSFORM_ROTATION | TRANSFORM_SCALE,
    COLLIDER_ALL = COLLIDER_SIZE | COLLIDER_SHAPE_TYPE | COLLIDER_TRIGGER,
    RIGIDBODY_ALL = RIGIDBODY_MAX_LINEAR_VELOCITY | RIGIDBODY_MAX_ANGULAR_VELOCITY | RIGIDBODY_BODY_TYPE | RIGIDBODY_GROUP_INDEX | RIGIDBODY_BULLET,
    LABEL_ALL = LABEL_OFFSET | LABEL_EXTRA_OFFSET | LABEL_TEXT | LABEL_COLOR,
    SPRITE_ALL = SPRITE_FILE | SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS | SPRITE_SPRITE_NAME | SPRITE_VISIBLE,
    WEAPON_ALL = WEAPON_DAMAGE_PRIMARY | WEAPON_DAMAGE_SECONDARY | WEAPON_CURRENT_AMMO_PRIMARY | WEAPON_MAX_AMMO_PRIMARY | WEAPON_AMMO_PRIMARY | WEAPON_RANGE_PRIMARY | WEAPON_RANGE_SECONDARY | WEAPON_TIME_SHOOT_PRIMARY | WEAPON_COOLDOWN_SHOOT_PRIMARY | WEAPON_TIME_SHOOT_SECONDARY | WEAPON_COOLDOWN_SHOOT_SECONDARY | WEAPON_TIME_RELOAD | WEAPON_COOLDOWN_RELOAD | WEAPON_ORIGIN_LAST_SHOT | WEAPON_DESTINATION_LAST_SHOT | WEAPON_HIT_ENTITY_LAST_SHOT,
    HEALTH_ALL = HEALTH_CURRENT_HP | HEALTH_MAX_HP | HEALTH_HP | HEALTH_TIME_HEAL | HEALTH_COOLDOWN_HEAL | HEALTH_HIT_ENTITY_LAST_SHOT | HEALTH_DIRECTION_LAST_SHOT,
    SIGHT_ALL = SIGHT_ANGLE | SIGHT_DISTANCE,
    BOT_ALL = BOT_ACTION_TYPE,

    ALL = TRANSFORM_ALL | COLLIDER_ALL | RIGIDBODY_ALL | LABEL_ALL | SPRITE_ALL | WEAPON_ALL | HEALTH_ALL | SIGHT_ALL | BOT_ALL
};
}

#endif
