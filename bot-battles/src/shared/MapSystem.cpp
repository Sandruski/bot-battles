#include "MapSystem.h"

#include "Game.h"
#include "MapComponent.h"
#include "RendererComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool MapSystem::DebugRender()
{
    std::weak_ptr<MapComponent> mapComponent = g_game->GetMapComponent();
    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();

    for (U32 i = 0; i < mapComponent.lock()->m_tileCount.x; ++i) {
        for (U32 j = 0; j < mapComponent.lock()->m_tileCount.y; ++j) {
            MapComponent::Tile& tile = mapComponent.lock()->GetTile(i, j);
            if (tile.m_tileType == MapComponent::Tile::TileType::NONE) {
                continue;
            }

            glm::vec2 tilePosition = mapComponent.lock()->MapToRealWorld(i, j);
            glm::vec3 position = glm::vec3(tilePosition.x, tilePosition.y, static_cast<F32>(LayerType::DEBUG));
            F32 rotation = 0.0f;
            glm::vec3 scale = glm::vec3(static_cast<F32>(mapComponent.lock()->m_tileSize.x), static_cast<F32>(mapComponent.lock()->m_tileSize.y), 0.0f);
            scale *= mapComponent.lock()->m_scale;

            glm::vec4 color = White;
            switch (tile.m_tileType) {
            case MapComponent::Tile::TileType::FLOOR: {
                color = Yellow;
                break;
            }
            case MapComponent::Tile::TileType::BOT_SPAWNER: {
                color = Red;
                break;
            }
            case MapComponent::Tile::TileType::WEAPON_SPAWNER: {
                color = Green;
                break;
            }
            case MapComponent::Tile::TileType::HEALTH_SPAWNER: {
                color = Blue;
                break;
            }
            default: {
                break;
            }
            }
            color.a = 0.5f;

            rendererComponent.lock()->DrawQuad(position, rotation, scale, color, true);
        }
    }

    return true;
}
}
