#include "MapImporter.h"

#include "AmmoSpawnerComponent.h"
#include "BotSpawnerComponent.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "FileSystem.h"
#include "Game.h"
#include "ResourceManager.h"
#include "RigidbodyComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WallComponent.h"
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
    WindowComponent& windowComponent = g_game->GetWindowComponent();

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
                glm::uvec2 tilePosition = tilemap.MapToWorld(i, j);
                glm::vec2 worldPosition = static_cast<glm::vec2>(tilePosition);
                worldPosition += static_cast<glm::vec2>(tilemap.m_tileSize) / 2.0f;
                worldPosition *= 2.0f;
                worldPosition -= static_cast<glm::vec2>(tilemap.m_tileSize * tilemap.m_tileCount);
                worldPosition *= 0.5f;
                worldPosition += static_cast<glm::vec2>(windowComponent.m_baseResolution) / 2.0f;
                std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().AddComponent<TransformComponent>(entity);
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
                    g_game->GetComponentManager().AddComponent<WallComponent>(entity);

                    // Collider
                    std::weak_ptr<ColliderComponent> colliderComponent = g_game->GetComponentManager().AddComponent<ColliderComponent>(entity);
                    colliderComponent.lock()->m_size = static_cast<glm::vec2>(tilemap.m_tileSize);
                    colliderComponent.lock()->m_shapeType = ColliderComponent::ShapeType::BOX;

                    // Rigidbody
                    std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_game->GetComponentManager().AddComponent<RigidbodyComponent>(entity);
                    rigidbodyComponent.lock()->m_bodyType = RigidbodyComponent::BodyType::STATIC;
                    rigidbodyComponent.lock()->UpdateBodyType();
                    rigidbodyComponent.lock()->m_groupIndex = 0;
                    rigidbodyComponent.lock()->UpdateGroupIndex();
                }
            }
        }
    }

    for (const auto& objectlayer : tilemap.m_objectlayers) {
        for (const auto& object : objectlayer.m_objects) {
            Entity entity = g_game->GetEntityManager().AddEntity();

            // SHARED
            // Transform
            glm::uvec2 objectPosition = object.m_position;
            glm::vec2 worldPosition = static_cast<glm::vec2>(objectPosition);
            worldPosition += static_cast<glm::vec2>(tilemap.m_tileSize) / 2.0f;
            worldPosition -= glm::vec2(0.0f, tilemap.m_tileSize.y);
            worldPosition *= 2.0f;
            worldPosition -= static_cast<glm::vec2>(tilemap.m_tileSize * tilemap.m_tileCount);
            worldPosition *= 0.5f;
            worldPosition += static_cast<glm::vec2>(windowComponent.m_baseResolution) / 2.0f;
            std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().AddComponent<TransformComponent>(entity);
            transformComponent.lock()->m_position = worldPosition;
            transformComponent.lock()->m_layerType = LayerType::OBJECT;
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
                std::weak_ptr<BotSpawnerComponent> botSpawnerComponent = g_game->GetComponentManager().AddComponent<BotSpawnerComponent>(entity);
                botSpawnerComponent.lock()->m_playerID = object.m_properties.front().m_value.intValue;
            }

            // AmmoSpawner
            if (object.m_type == "AmmoSpawner") {
                std::weak_ptr<AmmoSpawnerComponent> ammoSpawnerComponent = g_game->GetComponentManager().AddComponent<AmmoSpawnerComponent>(entity);
                ammoSpawnerComponent.lock()->m_ammo = object.m_properties.front().m_value.intValue;

                // Collider
                std::weak_ptr<ColliderComponent> colliderComponent = g_game->GetComponentManager().AddComponent<ColliderComponent>(entity);
                colliderComponent.lock()->m_size = static_cast<glm::vec2>(tilemap.m_tileSize);
                colliderComponent.lock()->m_shapeType = ColliderComponent::ShapeType::BOX;

                // Rigidbody
                std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_game->GetComponentManager().AddComponent<RigidbodyComponent>(entity);
                rigidbodyComponent.lock()->m_bodyType = RigidbodyComponent::BodyType::STATIC;
                rigidbodyComponent.lock()->UpdateBodyType();
                rigidbodyComponent.lock()->m_groupIndex = 0;
                rigidbodyComponent.lock()->UpdateGroupIndex();
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
