#include "MapRendererSystem.h"

#include "ComponentManager.h"
#include "Game.h"
#include "MapComponent.h"
#include "MeshResource.h"
#include "RendererComponent.h"
#include "ShaderResource.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MapRendererSystem::MapRendererSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::MAP);
}

//----------------------------------------------------------------------------------------------------
bool MapRendererSystem::Render()
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    WindowComponent& windowComponent = g_game->GetWindowComponent();

    for (auto& entity : m_entities) {

        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<MapComponent> mapComponent = g_game->GetComponentManager().GetComponent<MapComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !mapComponent.lock()->m_isEnabled) {
            continue;
        }

        // TODO: move center of tile so top-left tile is not half-drawn.

        for (const auto& tileLayer : mapComponent.lock()->m_tileLayers) {
            for (U32 i = 0; i < mapComponent.lock()->m_size.x; ++i) {
                for (U32 j = 0; j < mapComponent.lock()->m_size.y; ++j) {
                    U32 tileGid = tileLayer.at(i + mapComponent.lock()->m_size.x * j);
                    if (tileGid == 0) {
                        continue;
                    }

                    MapComponent::Tileset tileset = mapComponent.lock()->m_tilesets.front();
                    for (const auto& elem : mapComponent.lock()->m_tilesets) {
                        if (tileGid < elem.m_firstGid) {
                            break;
                        }
                        tileset = elem;
                    }

                    glm::uvec4 textureCoords = tileset.GetTileTextureCoords(tileGid);
                    glm::uvec2 position = mapComponent.lock()->MapToWorld(i, j);
                    glm::vec2 realPosition = glm::vec2(position.x, position.y);
                    realPosition += glm::vec2(transformComponent.lock()->m_position.x, transformComponent.lock()->m_position.y);
                    realPosition -= glm::vec2(static_cast<F32>(mapComponent.lock()->m_size.x * mapComponent.lock()->m_tileSize.x) / 2.0f,
                        static_cast<F32>(mapComponent.lock()->m_size.y * mapComponent.lock()->m_tileSize.y) / 2);
                    realPosition += glm::vec2(mapComponent.lock()->m_tileSize.x / 2, mapComponent.lock()->m_tileSize.y / 2);
                    ILOG("Tile gid %u", tileGid);
                    ILOG("Tex coords %u and %u", textureCoords.x, textureCoords.y);
                    ILOG("Position %u and %u", position.x, position.y);
                    // Render tile
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(realPosition.x, realPosition.y, 0.0f));
                    model = glm::rotate(model, glm::radians(transformComponent.lock()->m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));

                    model = glm::scale(model, glm::vec3(static_cast<F32>(textureCoords.z), static_cast<F32>(textureCoords.w), 0.0f));

                    glm::uvec2 size = tileset.m_spriteResource.lock()->GetSize();
                    std::array<Vertex, 4> vertices = rendererComponent.m_meshResource.lock()->GetVertices();
                    // Top-left
                    vertices[0].m_textureCoords = glm::vec2(textureCoords.x / static_cast<F32>(size.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(size.y));
                    // Top-right
                    vertices[1].m_textureCoords = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(size.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(size.y));
                    // Bottom-left
                    vertices[2].m_textureCoords = glm::vec2(textureCoords.x / static_cast<F32>(size.x), 1.0f - textureCoords.y / static_cast<F32>(size.y));
                    // Bottom-right
                    vertices[3].m_textureCoords = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(size.x), 1.0f - textureCoords.y / static_cast<F32>(size.y));
                    rendererComponent.m_meshResource.lock()->ReLoad(vertices);

                    U32 modelLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "model");
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_resolution.x), static_cast<F32>(windowComponent.m_resolution.y), 0.0f, -1.0f, 1.0f);
                    U32 projectionLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "projection");
                    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, tileset.m_spriteResource.lock()->GetTexture());

                    glBindVertexArray(rendererComponent.m_meshResource.lock()->GetVAO());
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                    glBindTexture(GL_TEXTURE_2D, 0);
                    glBindVertexArray(0);
                }
            }
        }
    }

    return true;
}
}
