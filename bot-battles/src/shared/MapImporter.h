#ifndef __MAP_IMPORTER_H__
#define __MAP_IMPORTER_H__

namespace sand {

struct MapComponent;

//----------------------------------------------------------------------------------------------------
class MapImporter {
public:
    bool Load(const std::string& path) const;
    //void UnLoad(U32& texture) const;

private:
    bool LoadTilesets(const rapidjson::Value& value, std::weak_ptr<MapComponent> mapComponent) const;
    void LoadLayers(const rapidjson::Value& value, std::weak_ptr<MapComponent> mapComponent) const;
    void LoadTileLayer(const rapidjson::Value& value, std::weak_ptr<MapComponent> mapComponent) const;
    void LoadObjectLayer(const rapidjson::Value& value) const;
};
}

#endif
