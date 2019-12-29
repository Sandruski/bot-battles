#include "RendererComponent.h"

#include "FontResource.h"
#include "Game.h"
#include "ResourceManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RendererComponent::RendererComponent()
    : m_renderer(nullptr)
    , m_backgroundColor()
    , m_isDebugDraw(false)
{
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::LoadFromConfig(const rapidjson::Value& value)
{
    assert(value.HasMember("defaultFont"));
    assert(value["defaultFont"].IsObject());
    const rapidjson::Value& font = value["defaultFont"];
    assert(font.HasMember("name"));
    assert(font["name"].IsString());
    const char* name = font["name"].GetString();
    assert(font.HasMember("size"));
    assert(font["size"].IsUint());
    U16 size = static_cast<U16>(font["size"].GetUint());
    m_defaultFont = g_game->GetResourceManager().AddResource<FontResource>(name, FONTS_DIR, true);
    m_defaultFont.lock()->m_size = size;
    m_defaultFont.lock()->ReLoad();

    assert(value.HasMember("backgroundColor"));
    assert(value["backgroundColor"].IsArray());
    assert(!value["backgroundColor"].Empty());
    assert(value["backgroundColor"][0].IsUint());
    m_backgroundColor.r = static_cast<U8>(value["backgroundColor"][0].GetUint());
    assert(value["backgroundColor"][1].IsUint());
    m_backgroundColor.g = static_cast<U8>(value["backgroundColor"][1].GetUint());
    assert(value["backgroundColor"][2].IsUint());
    m_backgroundColor.b = static_cast<U8>(value["backgroundColor"][2].GetUint());
    assert(value["backgroundColor"][3].IsUint());
    m_backgroundColor.a = static_cast<U8>(value["backgroundColor"][3].GetUint());

    assert(value.HasMember("debugDraw"));
    assert(value["debugDraw"].IsBool());
    m_isDebugDraw = value["debugDraw"].GetBool();
}
}
