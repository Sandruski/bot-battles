#include "RendererComponent.h"

#include "Game.h"
#include "MeshResource.h"
#include "ResourceManager.h"
#include "ShaderResource.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RendererComponent::RendererComponent()
    : m_defaultShaderResource()
    , m_instancingShaderResource()
    , m_lineMeshResource()
    , m_circleMeshResource()
    , m_quadMeshResource()
    , m_mapMeshResource()
    , m_charactersMeshResource()
    , m_objectsMeshResource()
    , m_backgroundColor(0.0f, 0.0f, 0.0f, 0.0f)
    , m_isVSync(true)
    , m_isDebugDraw(false)
    , m_isDebugDrawColliders(true)
    , m_isDebugDrawBot(true)
    , m_isDebugDrawMap(true)
{
}

//----------------------------------------------------------------------------------------------------
RendererComponent::~RendererComponent()
{
    if (!m_defaultShaderResource.expired()) {
        g_game->GetResourceManager().RemoveResource<ShaderResource>(m_defaultShaderResource.lock()->GetID());
    }
    if (!m_instancingShaderResource.expired()) {
        g_game->GetResourceManager().RemoveResource<ShaderResource>(m_instancingShaderResource.lock()->GetID());
    }

    if (!m_lineMeshResource.expired()) {
        g_game->GetResourceManager().RemoveResource<ShaderResource>(m_lineMeshResource.lock()->GetID());
    }
    if (!m_circleMeshResource.expired()) {
        g_game->GetResourceManager().RemoveResource<ShaderResource>(m_circleMeshResource.lock()->GetID());
    }
    if (!m_quadMeshResource.expired()) {
        g_game->GetResourceManager().RemoveResource<ShaderResource>(m_quadMeshResource.lock()->GetID());
    }
    if (!m_mapMeshResource.expired()) {
        g_game->GetResourceManager().RemoveResource<ShaderResource>(m_mapMeshResource.lock()->GetID());
    }
    if (!m_charactersMeshResource.expired()) {
        g_game->GetResourceManager().RemoveResource<ShaderResource>(m_charactersMeshResource.lock()->GetID());
    }
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::LoadFromConfig(const rapidjson::Value& value)
{
    if (value.HasMember("VSync") && value["VSync"].IsBool()) {
        m_isVSync = value["VSync"].GetBool();
    }
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::SetWireframe(bool wireframe) const
{
    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

//----------------------------------------------------------------------------------------------------
bool RendererComponent::UpdateVSync() const
{
    if (SDL_GL_SetSwapInterval(m_isVSync) == -1) {
        ELOG("VSync could not be updated");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::UpdateBackgroundColor() const
{
    glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::DrawLine(glm::vec2 fromPosition, glm::vec2 toPosition, F32 positionZ, glm::vec4 color)
{
    glUseProgram(m_defaultShaderResource.lock()->GetProgram());

    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.lock()->GetProportion();

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 position = glm::vec3(0.0f, 0.0f, positionZ);
    model = glm::translate(model, position);

    std::vector<MeshResource::Vertex> lineVertices = MeshResource::GetLineVertices(fromPosition, toPosition);
    m_lineMeshResource.lock()->ReLoadVertices(lineVertices);

    U32 modelLoc = glGetUniformLocation(m_defaultShaderResource.lock()->GetProgram(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.x), -static_cast<F32>(windowComponent.lock()->m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_defaultShaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 colorLoc = glGetUniformLocation(m_defaultShaderResource.lock()->GetProgram(), "color");
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));

    glBindVertexArray(m_lineMeshResource.lock()->GetVAO());
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_lineMeshResource.lock()->GetVertices().size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::DrawCircle(glm::vec3 position, F32 rotation, glm::vec3 scale, U32 sides, F32 angle, F32 radius, glm::vec4 color, bool filled)
{
    glUseProgram(m_defaultShaderResource.lock()->GetProgram());

    if (!filled) {
        SetWireframe(true);
    }

    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.lock()->GetProportion();

    glm::mat4 model = glm::mat4(1.0f);
    position.x *= proportion.x;
    position.y *= proportion.y;
    position.y *= -1.0f;
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, -1.0f));
    scale.x *= proportion.x;
    scale.y *= proportion.y;
    model = glm::scale(model, scale);

    std::vector<MeshResource::Vertex> circleVertices = MeshResource::GetCircleVertices(sides, angle, radius);
    m_circleMeshResource.lock()->ReLoadVertices(circleVertices);

    U32 modelLoc = glGetUniformLocation(m_defaultShaderResource.lock()->GetProgram(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.x), -static_cast<F32>(windowComponent.lock()->m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_defaultShaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 colorLoc = glGetUniformLocation(m_defaultShaderResource.lock()->GetProgram(), "color");
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));

    glBindVertexArray(m_circleMeshResource.lock()->GetVAO());
    glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(m_circleMeshResource.lock()->GetVertices().size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    if (!filled) {
        SetWireframe(false);
    }
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::DrawQuad(glm::vec3 position, F32 rotation, glm::vec3 scale, glm::vec4 color, bool filled)
{
    glUseProgram(m_defaultShaderResource.lock()->GetProgram());

    if (!filled) {
        SetWireframe(true);
    }

    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.lock()->GetProportion();

    glm::mat4 model = glm::mat4(1.0f);
    position.x *= proportion.x;
    position.y *= proportion.y;
    position.y *= -1.0f;
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, -1.0f));
    scale.x *= proportion.x;
    scale.y *= proportion.y;
    model = glm::scale(model, scale);

    U32 modelLoc = glGetUniformLocation(m_defaultShaderResource.lock()->GetProgram(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.x), -static_cast<F32>(windowComponent.lock()->m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_defaultShaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 colorLoc = glGetUniformLocation(m_defaultShaderResource.lock()->GetProgram(), "color");
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));

    glBindVertexArray(m_quadMeshResource.lock()->GetVAO());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(m_quadMeshResource.lock()->GetVertices().size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    if (!filled) {
        SetWireframe(false);
    }
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::DrawMapTexturedQuad(U32 texture)
{
    glUseProgram(m_instancingShaderResource.lock()->GetProgram());

    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.x), -static_cast<F32>(windowComponent.lock()->m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_instancingShaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 pctLoc = glGetUniformLocation(m_instancingShaderResource.lock()->GetProgram(), "pct");
    glUniform1f(pctLoc, 0.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(m_mapMeshResource.lock()->GetVAO());
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(m_mapMeshResource.lock()->GetVertices().size()), static_cast<GLsizei>(m_mapMeshResource.lock()->GetInstances().size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::DrawCharactersTexturedQuad(U32 texture)
{
    glUseProgram(m_instancingShaderResource.lock()->GetProgram());

    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.x), -static_cast<F32>(windowComponent.lock()->m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_instancingShaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 pctLoc = glGetUniformLocation(m_instancingShaderResource.lock()->GetProgram(), "pct");
    glUniform1f(pctLoc, 0.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(m_charactersMeshResource.lock()->GetVAO());
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(m_charactersMeshResource.lock()->GetVertices().size()), static_cast<GLsizei>(m_charactersMeshResource.lock()->GetInstances().size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::DrawObjectsTexturedQuad(U32 texture)
{
    glUseProgram(m_instancingShaderResource.lock()->GetProgram());

    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.x), -static_cast<F32>(windowComponent.lock()->m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_instancingShaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 pctLoc = glGetUniformLocation(m_instancingShaderResource.lock()->GetProgram(), "pct");
    glUniform1f(pctLoc, 0.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(m_objectsMeshResource.lock()->GetVAO());
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(m_objectsMeshResource.lock()->GetVertices().size()), static_cast<GLsizei>(m_objectsMeshResource.lock()->GetInstances().size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}
}
