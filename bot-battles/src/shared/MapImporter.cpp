#include "MapImporter.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "FileSystem.h"
#include "Game.h"
#include "MapComponent.h"
#include "ResourceManager.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
U32 MapImporter::Load(const std::string& path) const
{
    rapidjson::Document document;
    bool ret = g_game->GetFileSystem().ParseJsonFromFile(path, document);
    if (!ret) {
        ELOG("%s file could not be loaded", path.c_str());
        return 0;
    }

    assert(document.IsObject());

    Entity entity = g_game->GetEntityManager().AddEntity();
    std::weak_ptr<MapComponent> mapComponent = g_game->GetComponentManager().AddComponent<MapComponent>(entity);

    assert(document.HasMember("width"));
    mapComponent.lock()->m_tileCount.x = document["width"].GetUint();
    assert(document.HasMember("height"));
    mapComponent.lock()->m_tileCount.y = document["height"].GetUint();

    assert(document.HasMember("tilewidth"));
    mapComponent.lock()->m_tileSize.x = document["tilewidth"].GetUint();
    assert(document.HasMember("tileheight"));
    mapComponent.lock()->m_tileSize.y = document["tileheight"].GetUint();

    assert(document.HasMember("tilesets"));
    const rapidjson::Value& tilesets = document["tilesets"];
    ret = LoadTilesets(tilesets, mapComponent);
    if (!ret) {
        g_game->GetEntityManager().RemoveEntity(entity);
        return 0;
    }

    assert(document.HasMember("layers"));
    const rapidjson::Value& layers = document["layers"];
    LoadLayers(layers, mapComponent);

    return entity;
}

//----------------------------------------------------------------------------------------------------
bool MapImporter::LoadTilesets(const rapidjson::Value& value, std::weak_ptr<MapComponent> mapComponent) const
{
    mapComponent.lock()->m_tilesets.reserve(value.Size());

    for (rapidjson::Value::ConstValueIterator it = value.Begin(); it != value.End(); ++it) {
        MapComponent::Tileset tileset;

        assert(it->HasMember("firstgid"));
        tileset.m_firstGid = (*it)["firstgid"].GetUint();

        assert(it->HasMember("source"));
        std::string source;
        source.append(MAPS_DIR);
        source.append((*it)["source"].GetString());

        rapidjson::Document document;
        bool ret = g_game->GetFileSystem().ParseJsonFromFile(source, document);
        if (!ret) {
            ELOG("%s file could not be loaded", source.c_str());
            return false;
        }

        assert(document.IsObject());

        assert(document.HasMember("image"));
        std::string image = document["image"].GetString();
        image = image.substr(image.find_last_of("/") + 1, image.size());
        tileset.m_spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>(image.c_str(), TEXTURES_DIR, true);

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

        mapComponent.lock()->m_tilesets.emplace_back(tileset);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void MapImporter::LoadLayers(const rapidjson::Value& value, std::weak_ptr<MapComponent> mapComponent) const
{
    for (rapidjson::Value::ConstValueIterator it = value.Begin(); it != value.End(); ++it) {
        assert(it->HasMember("type"));
        std::string type = (*it)["type"].GetString();

        if (type == "tilelayer") {
            LoadTileLayer(*it, mapComponent);
        } else if (type == "objectgroup") {
            //LoadObjectLayer(*it);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void MapImporter::LoadTileLayer(const rapidjson::Value& value, std::weak_ptr<MapComponent> mapComponent) const
{
    MapComponent::TileLayer tileLayer;

    assert(value.HasMember("data"));
    tileLayer.m_data.reserve(value["data"].Size());
    for (rapidjson::Value::ConstValueIterator it = value["data"].Begin(); it != value["data"].End(); ++it) {
        tileLayer.m_data.emplace_back(it->GetUint());
    }

    mapComponent.lock()->m_tileLayers.emplace_back(tileLayer);
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
