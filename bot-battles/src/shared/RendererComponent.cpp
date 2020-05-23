#include "RendererComponent.h"

#include "Game.h"
#include "MeshResource.h"
#include "ResourceManager.h"
#include "ShaderResource.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RendererComponent::RendererComponent()
    : m_shaderResource()
    , m_lineMeshResource()
    , m_circleMeshResource()
    , m_quadMeshResource()
    , m_backgroundColor(0.0f, 0.0f, 0.0f, 0.0f)
    , m_isVSync(true)
    , m_isDebugDraw(true)
{
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::LoadFromConfig(const rapidjson::Value& value)
{
    assert(value.HasMember("VSync"));
    m_isVSync = value["VSync"].GetBool();
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
void RendererComponent::DrawLine(const glm::vec3& fromPosition, const glm::vec3& toPosition, const glm::vec4& color)
{
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

    glm::mat4 model = glm::mat4(1.0f);

    std::vector<MeshResource::Vertex> lineVertices = MeshResource::GetLineVertices(fromPosition, toPosition);
    m_lineMeshResource.lock()->ReLoad(lineVertices);

    U32 modelLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_currentResolution.x), -static_cast<F32>(windowComponent.m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 colorLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "color");
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));

    U32 pctLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "pct");
    glUniform1f(pctLoc, 1.0f);

    glBindVertexArray(m_lineMeshResource.lock()->GetVAO());
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_lineMeshResource.lock()->GetVertices().size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::DrawCircle(glm::vec3& position, F32 rotation, glm::vec3& scale, U32 sides, F32 angle, F32 radius, const glm::vec4& color, bool filled)
{
    if (!filled) {
        SetWireframe(true);
    }

    WindowComponent& windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

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
    m_circleMeshResource.lock()->ReLoad(circleVertices);

    U32 modelLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_currentResolution.x), -static_cast<F32>(windowComponent.m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 colorLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "color");
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));

    U32 pctLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "pct");
    glUniform1f(pctLoc, 1.0f);

    glBindVertexArray(m_circleMeshResource.lock()->GetVAO());
    glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(m_circleMeshResource.lock()->GetVertices().size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    if (!filled) {
        SetWireframe(false);
    }
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::DrawQuad(glm::vec3& position, F32 rotation, glm::vec3& scale, const glm::vec4& color, bool filled)
{
    if (!filled) {
        SetWireframe(true);
    }

    WindowComponent& windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

    glm::mat4 model = glm::mat4(1.0f);
    position.x *= proportion.x;
    position.y *= proportion.y;
    position.y *= -1.0f;
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, -1.0f));
    scale.x *= proportion.x;
    scale.y *= proportion.y;
    model = glm::scale(model, scale);

    U32 modelLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_currentResolution.x), -static_cast<F32>(windowComponent.m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 colorLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "color");
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));

    U32 pctLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "pct");
    glUniform1f(pctLoc, 1.0f);

    glBindVertexArray(m_quadMeshResource.lock()->GetVAO());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(m_quadMeshResource.lock()->GetVertices().size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    if (!filled) {
        SetWireframe(false);
    }
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::DrawTexturedQuad(const std::vector<glm::mat4>& models, const std::vector<glm::vec2>& texCoords, U32 texture, const glm::vec4& color, F32 pct)
{
    WindowComponent& windowComponent = g_game->GetWindowComponent();

    U32 modelLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "model[0]");
    glUniformMatrix4fv(modelLoc, static_cast<GLsizei>(models.size()), GL_FALSE, glm::value_ptr(models.at(0)));

    U32 texCoordsLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "texCoords[0]");
    glUniform2fv(texCoordsLoc, static_cast<GLsizei>(texCoords.size()), glm::value_ptr(texCoords.at(0)));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_currentResolution.x), -static_cast<F32>(windowComponent.m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 colorLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "color");
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));

    U32 pctLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "pct");
    glUniform1f(pctLoc, pct);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(m_quadMeshResource.lock()->GetVAO());
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(m_quadMeshResource.lock()->GetVertices().size()), static_cast<GLsizei>(models.size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}
}
