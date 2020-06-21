#include "MapImporter.h"

#include "BotSpawnerComponent.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "FileSystem.h"
#include "Game.h"
#include "HealthSpawnerComponent.h"
#include "LinkingContext.h"
#include "MapComponent.h"
#include "RendererComponent.h"
#include "ResourceManager.h"
#include "RigidbodyComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WallComponent.h"
#include "WeaponSpawnerComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool MapImporter::Load(const std::string& path, Tilemap& tilemap) const
{
    rapidjson::Document document;
    bool ret = g_game->GetFileSystem().ParseJsonFromFile(path, document);
    if (!ret) {
        ELOG("%s file could not be loaded", path.c_str());
        return ret;
    }
    ret = document.IsObject();
    if (!ret) {
        return ret;
    }

    ret = document.HasMember("width");
    if (!ret) {
        return ret;
    }
    tilemap.m_tileCount.x = document["width"].GetUint();
    ret = document.HasMember("height");
    if (!ret) {
        return ret;
    }
    tilemap.m_tileCount.y = document["height"].GetUint();
    ret = document.HasMember("tilewidth");
    if (!ret) {
        return ret;
    }
    tilemap.m_tileSize.x = document["tilewidth"].GetUint();
    ret = document.HasMember("tileheight");
    if (!ret) {
        return ret;
    }
    tilemap.m_tileSize.y = document["tileheight"].GetUint();

    ret = document.HasMember("tilesets");
    if (!ret) {
        return ret;
    }
    const rapidjson::Value& tilesetsValue = document["tilesets"];
    ret = LoadTilesets(tilesetsValue, tilemap.m_tilesets);
    if (!ret) {
        return ret;
    }

    ret = document.HasMember("layers");
    if (!ret) {
        return ret;
    }
    for (rapidjson::Value::ConstValueIterator it = document["layers"].Begin(); it != document["layers"].End(); ++it) {
        ret = it->HasMember("type");
        if (!ret) {
            return ret;
        }
        std::string type = (*it)["type"].GetString();
        if (type == "tilelayer") {
            Tilelayer tilelayer;
            ret = LoadTilelayer(*it, tilelayer);
            if (!ret) {
                return ret;
            }
            tilemap.m_tilelayers.emplace_back(tilelayer);
        } else if (type == "objectgroup") {
            Objectlayer objectlayer;
            ret = LoadObjectLayer(*it, objectlayer);
            if (!ret) {
                return ret;
            }
            tilemap.m_objectlayers.emplace_back(objectlayer);
        }
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
void MapImporter::Create(const Tilemap& tilemap) const
{
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    std::weak_ptr<MapComponent> mapComponent = g_game->GetMapComponent();
    mapComponent.lock()->m_tileCount = tilemap.m_tileCount;
    mapComponent.lock()->m_tileSize = tilemap.m_tileSize;
    glm::uvec2 mapSize = mapComponent.lock()->GetMapSize();
    glm::vec2 proportion = static_cast<glm::vec2>(windowComponent.lock()->m_baseResolution) / static_cast<glm::vec2>(mapSize);
    mapComponent.lock()->m_mapScale = proportion.x <= proportion.y ? proportion.x : proportion.y;
    U32 tileCount = mapComponent.lock()->m_tileCount.x * mapComponent.lock()->m_tileCount.y;

    mapComponent.lock()->m_tileTypes.resize(tileCount);
    for (auto& tileType : mapComponent.lock()->m_tileTypes) {
        // Walkability
        tileType = MapComponent::TileType::NONE;
    }

    for (const auto& tilelayer : tilemap.m_tilelayers) {
        for (U32 i = 0; i < tilemap.m_tileCount.x; ++i) {
            for (U32 j = 0; j < tilemap.m_tileCount.y; ++j) {
                Entity entity = g_game->GetEntityManager().AddEntity();

                U32 tileGid = tilelayer.GetTileGid(i, j, tilemap.m_tileCount.x);
                if (tileGid == 0) {
                    continue;
                }

                // SHARED
                // Transform
                std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().AddComponent<TransformComponent>(entity);
                transformComponent.lock()->m_scale = mapComponent.lock()->m_mapScale;
                glm::uvec2 tilePosition = tilemap.MapToWorld(i, j);
                glm::vec2 worldPosition = static_cast<glm::vec2>(tilePosition);
                worldPosition += static_cast<glm::vec2>(tilemap.m_tileSize) / 2.0f;
                worldPosition *= 2.0f;
                worldPosition -= static_cast<glm::vec2>(tilemap.m_tileSize * tilemap.m_tileCount);
                worldPosition *= 0.5f;
                worldPosition *= transformComponent.lock()->m_scale;
                worldPosition += static_cast<glm::vec2>(windowComponent.lock()->m_baseResolution) / 2.0f;
                transformComponent.lock()->m_position = worldPosition;
                transformComponent.lock()->m_layerType = LayerType::FLOOR;

                // Sprite
                const Tileset& tileset = tilemap.GetTileset(tileGid);
                glm::uvec4 textureCoords = tileset.GetTextureCoords(tileGid);
                std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().AddComponent<SpriteComponent>(entity);
                spriteComponent.lock()->m_spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>(tileset.m_spriteFile.c_str(), TEXTURES_DIR, true);
                spriteComponent.lock()->AddSprite("default", textureCoords);

                // UNIQUE
                // Wall
                if (tilelayer.m_name == "wall") {
                    // Walkability
                    glm::uvec2 mapPosition = mapComponent.lock()->RealWorldToMap(transformComponent.lock()->m_position);
                    MapComponent::TileType& tileType = mapComponent.lock()->GetTileType(mapPosition);
                    tileType = MapComponent::TileType::WALL;
                    g_game->GetComponentManager().AddComponent<WallComponent>(entity);

                    // Collider
                    std::weak_ptr<ColliderComponent> colliderComponent = g_game->GetComponentManager().AddComponent<ColliderComponent>(entity);
                    colliderComponent.lock()->m_size = static_cast<glm::vec2>(tilemap.m_tileSize);
                    colliderComponent.lock()->m_size *= transformComponent.lock()->m_scale;
                    colliderComponent.lock()->m_shapeType = ColliderComponent::ShapeType::BOX;

                    // Rigidbody
                    std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_game->GetComponentManager().AddComponent<RigidbodyComponent>(entity);
                    rigidbodyComponent.lock()->m_bodyType = RigidbodyComponent::BodyType::STATIC;
                    rigidbodyComponent.lock()->UpdateBodyType();
                    rigidbodyComponent.lock()->m_groupIndex = 0;
                    rigidbodyComponent.lock()->UpdateGroupIndex();
                } else {
                    // Walkability
                    glm::uvec2 mapPosition = mapComponent.lock()->RealWorldToMap(transformComponent.lock()->m_position);
                    MapComponent::TileType& tileType = mapComponent.lock()->GetTileType(mapPosition);
                    tileType = MapComponent::TileType::FLOOR;
                }
            }
        }
    }

    for (const auto& objectlayer : tilemap.m_objectlayers) {
        for (const auto& object : objectlayer.m_objects) {
            Entity entity = g_game->GetEntityManager().AddEntity();

            // SHARED
            // Transform
            std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().AddComponent<TransformComponent>(entity);
            transformComponent.lock()->m_scale = mapComponent.lock()->m_mapScale;
            glm::uvec2 objectPosition = object.m_position;
            glm::vec2 worldPosition = static_cast<glm::vec2>(objectPosition);
            worldPosition += static_cast<glm::vec2>(tilemap.m_tileSize) / 2.0f;
            worldPosition -= glm::vec2(0.0f, tilemap.m_tileSize.y);
            worldPosition *= 2.0f;
            worldPosition -= static_cast<glm::vec2>(tilemap.m_tileSize * tilemap.m_tileCount);
            worldPosition *= 0.5f;
            worldPosition *= transformComponent.lock()->m_scale;
            worldPosition += static_cast<glm::vec2>(windowComponent.lock()->m_baseResolution) / 2.0f;
            transformComponent.lock()->m_position = worldPosition;
            transformComponent.lock()->m_layerType = LayerType::FLOOR;
            transformComponent.lock()->m_rotation = object.m_rotation;

            // Sprite
            U32 objectGid = object.m_gid;
            if (objectGid != 0) {
                std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().AddComponent<SpriteComponent>(entity);
                const Tileset& tileset = tilemap.GetTileset(objectGid);
                glm::uvec4 textureCoords = tileset.GetTextureCoords(objectGid);
                spriteComponent.lock()->m_spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>(tileset.m_spriteFile.c_str(), TEXTURES_DIR, true);
                spriteComponent.lock()->AddSprite("default", textureCoords);
            }

            // UNIQUE
            // BotSpawner
            if (object.m_type == "BotSpawner") {
                // Walkability
                glm::uvec2 mapPosition = mapComponent.lock()->RealWorldToMap(transformComponent.lock()->m_position);
                MapComponent::TileType& tileType = mapComponent.lock()->GetTileType(mapPosition);
                tileType = MapComponent::TileType::BOT_SPAWNER;

#ifdef _SERVER
                std::weak_ptr<BotSpawnerComponent> botSpawnerComponent = g_game->GetComponentManager().AddComponent<BotSpawnerComponent>(entity);
                for (const auto& property : object.m_properties) {
                    if (property.m_name == "player") {
                        botSpawnerComponent.lock()->m_playerID = property.m_value.intValue;
                    } else if (property.m_name == "transformRotation") {
                        botSpawnerComponent.lock()->m_transformRotation = property.m_value.floatValue;
                    } else if (property.m_name == "rigidbodyMaxLinearVelocity") {
                        botSpawnerComponent.lock()->m_rigidbodyMaxLinearVelocity = property.m_value.floatValue;
                    } else if (property.m_name == "rigidbodyMaxAngularVelocity") {
                        botSpawnerComponent.lock()->m_rigidbodyMaxAngularVelocity = property.m_value.floatValue;
                    } else if (property.m_name == "weaponDamage") {
                        botSpawnerComponent.lock()->m_weaponDamage = property.m_value.intValue;
                    } else if (property.m_name == "weaponRange") {
                        botSpawnerComponent.lock()->m_weaponRange = property.m_value.floatValue;
                    } else if (property.m_name == "weaponTimeShoot") {
                        botSpawnerComponent.lock()->m_weaponTimeShoot = property.m_value.floatValue;
                    } else if (property.m_name == "weaponCooldownShoot") {
                        botSpawnerComponent.lock()->m_weaponCooldownShoot = property.m_value.floatValue;
                    } else if (property.m_name == "healthMaxHP") {
                        botSpawnerComponent.lock()->m_healthMaxHP = property.m_value.intValue;
                    } else if (property.m_name == "sightAngle") {
                        botSpawnerComponent.lock()->m_sightAngle = property.m_value.floatValue;
                    } else if (property.m_name == "sightDistance") {
                        botSpawnerComponent.lock()->m_sightDistance = property.m_value.floatValue;
                    }
                }
#endif
            }

            // WeaponSpawner
            if (object.m_type == "WeaponSpawner") {
                // Walkability
                glm::uvec2 mapPosition = mapComponent.lock()->RealWorldToMap(transformComponent.lock()->m_position);
                MapComponent::TileType& tileType = mapComponent.lock()->GetTileType(mapPosition);
                tileType = MapComponent::TileType::WEAPON_SPAWNER;

#ifdef _SERVER
                std::weak_ptr<WeaponSpawnerComponent> weaponSpawnerComponent = g_game->GetComponentManager().AddComponent<WeaponSpawnerComponent>(entity);
                for (const auto& property : object.m_properties) {
                    if (property.m_name == "weaponDamage") {
                        weaponSpawnerComponent.lock()->m_weaponDamage = property.m_value.intValue;
                    } else if (property.m_name == "weaponMaxAmmo") {
                        weaponSpawnerComponent.lock()->m_weaponMaxAmmo = property.m_value.intValue;
                    } else if (property.m_name == "weaponAmmo") {
                        weaponSpawnerComponent.lock()->m_weaponAmmo = property.m_value.intValue;
                    } else if (property.m_name == "weaponRange") {
                        weaponSpawnerComponent.lock()->m_weaponRange = property.m_value.floatValue;
                    } else if (property.m_name == "weaponTimeShoot") {
                        weaponSpawnerComponent.lock()->m_weaponTimeShoot = property.m_value.floatValue;
                    } else if (property.m_name == "weaponCooldownShoot") {
                        weaponSpawnerComponent.lock()->m_weaponCooldownShoot = property.m_value.floatValue;
                    } else if (property.m_name == "weaponTimeReload") {
                        weaponSpawnerComponent.lock()->m_weaponTimeReload = property.m_value.floatValue;
                    } else if (property.m_name == "weaponCooldownReload") {
                        weaponSpawnerComponent.lock()->m_weaponCooldownReload = property.m_value.floatValue;
                    } else if (property.m_name == "amountSpawn") {
                        weaponSpawnerComponent.lock()->m_amountSpawn = property.m_value.intValue;
                    } else if (property.m_name == "cooldownSpawn") {
                        weaponSpawnerComponent.lock()->m_cooldownSpawn = property.m_value.floatValue;
                    }
                }
#endif
            }

            if (object.m_type == "HealthSpawner") {
                // Walkability
                glm::uvec2 mapPosition = mapComponent.lock()->RealWorldToMap(transformComponent.lock()->m_position);
                MapComponent::TileType& tileType = mapComponent.lock()->GetTileType(mapPosition);
                tileType = MapComponent::TileType::HEALTH_SPAWNER;

#ifdef _SERVER
                std::weak_ptr<HealthSpawnerComponent> healthSpawnerComponent = g_game->GetComponentManager().AddComponent<HealthSpawnerComponent>(entity);
                for (const auto& property : object.m_properties) {
                    if (property.m_name == "healthHP") {
                        healthSpawnerComponent.lock()->m_healthHP = property.m_value.intValue;
                    } else if (property.m_name == "healthTimeHeal") {
                        healthSpawnerComponent.lock()->m_healthTimeHeal = property.m_value.floatValue;
                    } else if (property.m_name == "healthCooldownHeal") {
                        healthSpawnerComponent.lock()->m_healthCooldownHeal = property.m_value.floatValue;
                    } else if (property.m_name == "amountSpawn") {
                        healthSpawnerComponent.lock()->m_amountSpawn = property.m_value.intValue;
                    } else if (property.m_name == "cooldownSpawn") {
                        healthSpawnerComponent.lock()->m_cooldownSpawn = property.m_value.floatValue;
                    }
                }
#endif
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
bool MapImporter::LoadTilesets(const rapidjson::Value& value, std::vector<Tileset>& tilesets) const
{
    bool ret = false;

    tilesets.reserve(value.Size());

    for (rapidjson::Value::ConstValueIterator it = value.Begin(); it != value.End(); ++it) {
        Tileset tileset;

        ret = it->HasMember("firstgid");
        if (!ret) {
            return ret;
        }
        tileset.m_firstGid = (*it)["firstgid"].GetUint();
        ret = it->HasMember("source");
        if (!ret) {
            return ret;
        }
        std::string source;
        source.append(TILESETS_DIR);
        source.append((*it)["source"].GetString());

        rapidjson::Document document;
        ret = g_game->GetFileSystem().ParseJsonFromFile(source, document);
        if (!ret) {
            ELOG("%s file could not be loaded", source.c_str());
            return ret;
        }
        ret = document.IsObject();
        if (!ret) {
            return ret;
        }

        ret = document.HasMember("image");
        if (!ret) {
            return ret;
        }
        std::string image = document["image"].GetString();
        image = image.substr(image.find_last_of("/") + 1, image.size());
        tileset.m_spriteFile = image;
        ret = document.HasMember("tilewidth");
        if (!ret) {
            return ret;
        }
        tileset.m_tileSize.x = document["tilewidth"].GetUint();
        ret = document.HasMember("tileheight");
        if (!ret) {
            return ret;
        }
        tileset.m_tileSize.y = document["tileheight"].GetUint();
        ret = document.HasMember("columns");
        if (!ret) {
            return ret;
        }
        tileset.m_tileCount.x = document["columns"].GetUint();
        ret = document.HasMember("tilecount");
        if (!ret) {
            return ret;
        }
        U32 tileCount = document["tilecount"].GetUint();
        tileset.m_tileCount.y = tileCount / tileset.m_tileCount.x;
        ret = document.HasMember("margin");
        if (!ret) {
            return ret;
        }
        tileset.m_margin = document["margin"].GetUint();
        ret = document.HasMember("spacing");
        if (!ret) {
            return ret;
        }
        tileset.m_spacing = document["spacing"].GetUint();

        tilesets.emplace_back(tileset);
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool MapImporter::LoadTilelayer(const rapidjson::Value& value, Tilelayer& tilelayer) const
{
    bool ret = value.HasMember("data");
    if (!ret) {
        return ret;
    }
    tilelayer.m_data.reserve(value["data"].Size());
    for (rapidjson::Value::ConstValueIterator it = value["data"].Begin(); it != value["data"].End(); ++it) {
        tilelayer.m_data.emplace_back(it->GetUint());
    }

    ret = value.HasMember("name");
    if (!ret) {
        return ret;
    }
    tilelayer.m_name = value["name"].GetString();

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool MapImporter::LoadObjectLayer(const rapidjson::Value& value, Objectlayer& objectlayer) const
{
    bool ret = value.HasMember("objects");
    if (!ret) {
        return ret;
    }
    objectlayer.m_objects.reserve(value["objects"].Size());
    for (rapidjson::Value::ConstValueIterator it = value["objects"].Begin(); it != value["objects"].End(); ++it) {
        Object object;
        ret = LoadObject(*it, object);
        if (!ret) {
            return ret;
        }
        objectlayer.m_objects.emplace_back(object);
    }

    ret = value.HasMember("name");
    if (!ret) {
        return ret;
    }
    objectlayer.m_name = value["name"].GetString();

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool MapImporter::LoadObject(const rapidjson::Value& value, Object& object) const
{
    bool ret = value.HasMember("x");
    if (!ret) {
        return ret;
    }
    object.m_position.x = value["x"].GetFloat();
    ret = value.HasMember("y");
    if (!ret) {
        return ret;
    }
    object.m_position.y = value["y"].GetFloat();
    ret = value.HasMember("width");
    if (!ret) {
        return ret;
    }
    object.m_size.x = value["width"].GetFloat();
    ret = value.HasMember("height");
    if (!ret) {
        return ret;
    }
    object.m_size.y = value["height"].GetFloat();
    ret = value.HasMember("rotation");
    if (!ret) {
        return ret;
    }
    object.m_rotation = value["rotation"].GetFloat();
    ret = value.HasMember("name");
    if (!ret) {
        return ret;
    }
    object.m_name = value["name"].GetString();
    ret = value.HasMember("type");
    if (!ret) {
        return ret;
    }
    object.m_type = value["type"].GetString();
    ret = value.HasMember("gid");
    if (!ret) {
        return ret;
    }
    object.m_gid = value["gid"].GetUint();
    ret = value.HasMember("properties");
    if (!ret) {
        return ret;
    }
    object.m_properties.reserve(value["properties"].Size());
    for (rapidjson::Value::ConstValueIterator it = value["properties"].Begin(); it != value["properties"].End(); ++it) {
        Property property;
        ret = LoadProperty(*it, property);
        if (!ret) {
            return ret;
        }
        object.m_properties.emplace_back(property);
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool MapImporter::LoadProperty(const rapidjson::Value& value, Property& property) const
{
    bool ret = value.HasMember("name");
    if (!ret) {
        return ret;
    }
    property.m_name = value["name"].GetString();
    ret = value.HasMember("value");
    if (!ret) {
        return ret;
    }
    ret = value.HasMember("type");
    if (!ret) {
        return ret;
    }
    std::string type = value["type"].GetString();
    if (type == "bool") {
        property.m_value.boolValue = value["value"].GetBool();
    } else if (type == "int") {
        property.m_value.intValue = value["value"].GetInt();
    } else if (type == "float") {
        property.m_value.floatValue = value["value"].GetFloat();
    }

    return ret;
}
}
