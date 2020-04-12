#include "MapImporter.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "FileSystem.h"
#include "Game.h"
#include "ResourceManager.h"
#include "SpawnComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MapImporter::Tilemap MapImporter::Load(const std::string& path) const
{
    Tilemap tilemap;

    rapidjson::Document document;
    bool ret = g_game->GetFileSystem().ParseJsonFromFile(path, document);
    if (!ret) {
        ELOG("%s file could not be loaded", path.c_str());
        return tilemap;
    }
    assert(document.IsObject());

    assert(document.HasMember("width"));
    tilemap.m_tileCount.x = document["width"].GetUint();
    assert(document.HasMember("height"));
    tilemap.m_tileCount.y = document["height"].GetUint();
    assert(document.HasMember("tilewidth"));
    tilemap.m_tileSize.x = document["tilewidth"].GetUint();
    assert(document.HasMember("tileheight"));
    tilemap.m_tileSize.y = document["tileheight"].GetUint();

    assert(document.HasMember("tilesets"));
    const rapidjson::Value& tilesetsValue = document["tilesets"];
    tilemap.m_tilesets = LoadTilesets(tilesetsValue);

    assert(document.HasMember("layers"));
    for (rapidjson::Value::ConstValueIterator it = document["layers"].Begin(); it != document["layers"].End(); ++it) {
        assert(it->HasMember("type"));
        std::string type = (*it)["type"].GetString();
        if (type == "tilelayer") {
            Tilelayer tilelayer = LoadTilelayer(*it);
            tilemap.m_tilelayers.emplace_back(tilelayer);
        } else if (type == "objectgroup") {
            Objectlayer objectlayer = LoadObjectLayer(*it);
            tilemap.m_objectlayers.emplace_back(objectlayer);
        }
    }

    return tilemap;
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

                // Collider
                if (tilelayer.m_name == "collision") {
                    std::weak_ptr<ColliderComponent> colliderComponent = g_game->GetComponentManager().AddComponent<ColliderComponent>(entity);
                    colliderComponent.lock()->m_size = tilemap.m_tileSize;
                }
            }
        }
    }

    for (const auto& objectlayer : tilemap.m_objectlayers) {
        for (const auto& object : objectlayer.m_objects) {
            Entity entity = g_game->GetEntityManager().AddEntity();

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

            // Collider
            if (objectlayer.m_name == "collision") {
                std::weak_ptr<ColliderComponent> colliderComponent = g_game->GetComponentManager().AddComponent<ColliderComponent>(entity);
                colliderComponent.lock()->m_size = object.m_size;
            }

            // Spawn
            if (objectlayer.m_name == "spawner") {
                g_game->GetComponentManager().AddComponent<SpawnComponent>(entity);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
std::vector<MapImporter::Tileset> MapImporter::LoadTilesets(const rapidjson::Value& value) const
{
    std::vector<Tileset> tilesets;
    tilesets.reserve(value.Size());

    for (rapidjson::Value::ConstValueIterator it = value.Begin(); it != value.End(); ++it) {
        Tileset tileset;

        assert(it->HasMember("firstgid"));
        tileset.m_firstGid = (*it)["firstgid"].GetUint();
        assert(it->HasMember("source"));
        std::string source;
        source.append(TILESETS_DIR);
        source.append((*it)["source"].GetString());

        rapidjson::Document document;
        bool ret = g_game->GetFileSystem().ParseJsonFromFile(source, document);
        if (!ret) {
            ELOG("%s file could not be loaded", source.c_str());
            break;
        }
        assert(document.IsObject());

        assert(document.HasMember("image"));
        std::string image = document["image"].GetString();
        image = image.substr(image.find_last_of("/") + 1, image.size());
        tileset.m_spriteFile = image;
        //tileset.m_spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>(image.c_str(), TEXTURES_DIR, true);
        assert(document.HasMember("tilewidth"));
        tileset.m_tileSize.x = document["tilewidth"].GetUint();
        assert(document.HasMember("tileheight"));
        tileset.m_tileSize.y = document["tileheight"].GetUint();
        assert(document.HasMember("columns"));
        tileset.m_tileCount.x = document["columns"].GetUint();
        assert(document.HasMember("tilecount"));
        U32 tileCount = document["tilecount"].GetUint();
        tileset.m_tileCount.y = tileCount / tileset.m_tileCount.x;
        assert(document.HasMember("margin"));
        tileset.m_margin = document["margin"].GetUint();
        assert(document.HasMember("spacing"));
        tileset.m_spacing = document["spacing"].GetUint();

        tilesets.emplace_back(tileset);
    }

    return tilesets;
}

//----------------------------------------------------------------------------------------------------
MapImporter::Tilelayer MapImporter::LoadTilelayer(const rapidjson::Value& value) const
{
    Tilelayer tilelayer;

    assert(value.HasMember("data"));
    tilelayer.m_data.reserve(value["data"].Size());
    for (rapidjson::Value::ConstValueIterator it = value["data"].Begin(); it != value["data"].End(); ++it) {
        tilelayer.m_data.emplace_back(it->GetUint());
    }

    assert(value.HasMember("name"));
    tilelayer.m_name = value["name"].GetString();

    return tilelayer;
}

//----------------------------------------------------------------------------------------------------
MapImporter::Objectlayer MapImporter::LoadObjectLayer(const rapidjson::Value& value) const
{
    Objectlayer objectlayer;

    assert(value.HasMember("objects"));
    objectlayer.m_objects.reserve(value["objects"].Size());
    for (rapidjson::Value::ConstValueIterator it = value["objects"].Begin(); it != value["objects"].End(); ++it) {
        Object object = LoadObject(*it);
        objectlayer.m_objects.emplace_back(object);
    }

    assert(value.HasMember("name"));
    objectlayer.m_name = value["name"].GetString();

    return objectlayer;
}

//----------------------------------------------------------------------------------------------------
MapImporter::Object MapImporter::LoadObject(const rapidjson::Value& value) const
{
    Object object;

    assert(value.HasMember("x"));
    object.m_position.x = value["x"].GetFloat();
    assert(value.HasMember("y"));
    object.m_position.y = value["y"].GetFloat();
    assert(value.HasMember("width"));
    object.m_size.x = value["width"].GetFloat();
    assert(value.HasMember("height"));
    object.m_size.y = value["height"].GetFloat();
    assert(value.HasMember("rotation"));
    object.m_rotation = value["rotation"].GetFloat();
    if (value.HasMember("gid")) {
        object.m_gid = value["gid"].GetUint();
    }

    return object;
}
}
