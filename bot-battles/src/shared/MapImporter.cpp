#include "MapImporter.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "FileSystem.h"
#include "Game.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool MapImporter::Load(const std::string& path) const
{
    rapidjson::Document document;
    bool ret = g_game->GetFileSystem().ParseJsonFromFile(path, document);
    if (!ret) {
        ELOG("%s file could not be loaded", path.c_str());
        return ret;
    }
    assert(document.IsObject());

    Tilemap tilemap;

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
            //LoadObjectLayer(*it);
        }
    }

    for (const auto& tilelayer : tilemap.m_tilelayers) {
        for (U32 i = 0; i < tilemap.m_tileCount.x; ++i) {
            for (U32 j = 0; j < tilemap.m_tileCount.y; ++j) {
                U32 tileGid = tilelayer.GetTileGid(i, j, tilemap.m_tileCount.x);
                if (tileGid == 0) {
                    continue;
                }
                const Tileset& tileset = tilemap.GetTileset(tileGid);
                glm::uvec4 textureCoords = tileset.GetTextureCoords(tileGid);

                glm::uvec2 position = tilemap.MapToWorld(i, j);
                position += tilemap.m_tileSize / 2u;

                Entity tile = g_game->GetEntityManager().AddEntity();
                std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().AddComponent<TransformComponent>(tile);
                transformComponent.lock()->m_position = glm::vec3(static_cast<F32>(position.x), static_cast<F32>(position.y), -1.0f);
                std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().AddComponent<SpriteComponent>(tile);
                spriteComponent.lock()->m_spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>(tileset.m_spriteFile.c_str(), TEXTURES_DIR, true);
                spriteComponent.lock()->AddSprite("default", textureCoords);
            }
        }
    }

    return true;
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

    return tilelayer;
}

//----------------------------------------------------------------------------------------------------
void MapImporter::LoadObjectLayer(const rapidjson::Value& value) const
{
    Entity entity = g_game->GetEntityManager().AddEntity();
    std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().AddComponent<TransformComponent>(entity);
    assert(value.HasMember("x"));
    transformComponent.lock()->m_position.x = value["x"].GetFloat();
    assert(value.HasMember("y"));
    transformComponent.lock()->m_position.y = value["y"].GetFloat();
    assert(value.HasMember("rotation"));
    transformComponent.lock()->m_rotation = value["rotation"].GetFloat();

    assert(value.HasMember("type"));
    std::string type = value["type"].GetString();

    if (type == "collision") {
        std::weak_ptr<ColliderComponent> colliderComponent = g_game->GetComponentManager().AddComponent<ColliderComponent>(entity);
        assert(value.HasMember("width"));
        colliderComponent.lock()->m_size.x = value["width"].GetFloat();
        assert(value.HasMember("height"));
        colliderComponent.lock()->m_size.y = value["height"].GetFloat();
    }
}
}
