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
    , m_meshResource()
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
void RendererComponent::DrawLine(glm::vec3 fromPosition, glm::vec3 toPosition, glm::vec4 color)
{
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

    glm::mat4 model = glm::mat4(1.0f);

    std::vector<MeshResource::Vertex> vertices = MeshResource::GetLineVertices(fromPosition, toPosition);
    m_meshResource.lock()->ReLoad(vertices);

    U32 modelLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_currentResolution.x), -static_cast<F32>(windowComponent.m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 colorLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "color");
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));

    U32 pctLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "pct");
    glUniform1f(pctLoc, 1.0f);

    glBindVertexArray(m_meshResource.lock()->GetVAO());
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::DrawCircle(glm::vec3 position, F32 rotation, glm::vec3 scale, F32 angle, F32 radius, glm::vec4 color, bool filled)
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

    std::vector<MeshResource::Vertex> vertices = MeshResource::GetCircleVertices(angle, radius);
    m_meshResource.lock()->ReLoad(vertices);

    U32 modelLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_currentResolution.x), -static_cast<F32>(windowComponent.m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 colorLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "color");
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));

    U32 pctLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "pct");
    glUniform1f(pctLoc, 1.0f);

    glBindVertexArray(m_meshResource.lock()->GetVAO());
    glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(vertices.size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    if (!filled) {
        SetWireframe(false);
    }
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::DrawQuad(glm::vec3 position, F32 rotation, glm::vec3 scale, glm::vec4 color, bool filled)
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

    std::vector<MeshResource::Vertex> vertices = MeshResource::GetQuadVertices();
    m_meshResource.lock()->ReLoad(vertices);

    U32 modelLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_currentResolution.x), -static_cast<F32>(windowComponent.m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 colorLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "color");
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));

    U32 pctLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "pct");
    glUniform1f(pctLoc, 1.0f);

    glBindVertexArray(m_meshResource.lock()->GetVAO());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(vertices.size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    if (!filled) {
        SetWireframe(false);
    }
}

//----------------------------------------------------------------------------------------------------
void RendererComponent::DrawTexturedQuad(
    const std::vector<glm::mat4>& models,
    const std::vector<glm::vec2>& textureCoords0,
    const std::vector<glm::vec2>& textureCoords1,
    const std::vector<glm::vec2>& textureCoords2,
    const std::vector<glm::vec2>& textureCoords3,
    U32 texture)
{
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

    U32 modelLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "model[0]");
    glUniformMatrix4fv(modelLoc, static_cast<GLsizei>(models.size()), GL_FALSE, glm::value_ptr(models.at(0)));

    U32 textureCoords0Loc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "textureCoords0[0]");
    glUniform2fv(textureCoords0Loc, static_cast<GLsizei>(textureCoords0.size()), glm::value_ptr(textureCoords0.at(0)));

    U32 textureCoords1Loc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "textureCoords1[0]");
    glUniform2fv(textureCoords1Loc, static_cast<GLsizei>(textureCoords1.size()), glm::value_ptr(textureCoords1.at(0)));

    U32 textureCoords2Loc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "textureCoords2[0]");
    glUniform2fv(textureCoords2Loc, static_cast<GLsizei>(textureCoords2.size()), glm::value_ptr(textureCoords2.at(0)));

    U32 textureCoords3Loc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "textureCoords3[0]");
    glUniform2fv(textureCoords3Loc, static_cast<GLsizei>(textureCoords3.size()), glm::value_ptr(textureCoords3.at(0)));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_currentResolution.x), -static_cast<F32>(windowComponent.m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
    U32 projectionLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    U32 pctLoc = glGetUniformLocation(m_shaderResource.lock()->GetProgram(), "pct");
    glUniform1f(pctLoc, 0.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(m_meshResource.lock()->GetVAO());
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(m_meshResource.lock()->GetVertices().size()), static_cast<GLsizei>(models.size()));

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}
}
