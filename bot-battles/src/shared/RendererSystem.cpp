#include "RendererSystem.h"

#include "Colors.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "Game.h"
#include "MeshResource.h"
#include "RendererComponent.h"
#include "ResourceManager.h"
#include "ShaderResource.h"
#include "SightComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RendererSystem::RendererSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::SPRITE);
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::StartUp()
{
    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();

    // GL 3.0 + GLSL 130
    //const char* glslVersion = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GLContext glContext = SDL_GL_CreateContext(windowComponent.lock()->m_window);
    SDL_GL_MakeCurrent(windowComponent.lock()->m_window, glContext);
    if (!rendererComponent.lock()->UpdateVSync()) {
        return false;
    }

    if (gl3wInit()) {
        ELOG("OpenGL could not be initialized");
        return false;
    }
    if (!gl3wIsSupported(3, 0)) {
        ELOG("OpenGL 3.0 is not supported");
        return false;
    }

    if (!windowComponent.lock()->UpdateDisplayMode()) {
        return false;
    }
    windowComponent.lock()->UpdateResolution();

    rendererComponent.lock()->UpdateBackgroundColor();
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    rendererComponent.lock()->m_defaultShaderResource = g_game->GetResourceManager().AddResource<ShaderResource>("", "", false);
    rendererComponent.lock()->m_defaultShaderResource.lock()->ForceLoad(defaultVertexShaderSource, defaultFragmentShaderSource);
    rendererComponent.lock()->m_instancingShaderResource = g_game->GetResourceManager().AddResource<ShaderResource>("", "", false);
    rendererComponent.lock()->m_instancingShaderResource.lock()->ForceLoad(instancingVertexShaderSource, instancingFragmentShaderSource);

    rendererComponent.lock()->m_lineMeshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    rendererComponent.lock()->m_lineMeshResource.lock()->ForceLoad();
    rendererComponent.lock()->m_circleMeshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    rendererComponent.lock()->m_circleMeshResource.lock()->ForceLoad();
    rendererComponent.lock()->m_quadMeshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    rendererComponent.lock()->m_quadMeshResource.lock()->ForceLoad();
    const std::vector<MeshResource::Vertex> quadVertices = MeshResource::GetQuadVertices();
    rendererComponent.lock()->m_quadMeshResource.lock()->ReLoadVertices(quadVertices);

    rendererComponent.lock()->m_backgroundMeshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    rendererComponent.lock()->m_backgroundMeshResource.lock()->ForceLoad();
    rendererComponent.lock()->m_backgroundMeshResource.lock()->ReLoadVertices(quadVertices);
    rendererComponent.lock()->m_backgroundMeshResource.lock()->m_isStatic = true;
    rendererComponent.lock()->m_mapMeshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    rendererComponent.lock()->m_mapMeshResource.lock()->ForceLoad();
    rendererComponent.lock()->m_mapMeshResource.lock()->ReLoadVertices(quadVertices);
    rendererComponent.lock()->m_mapMeshResource.lock()->m_isStatic = true;
    rendererComponent.lock()->m_charactersMeshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    rendererComponent.lock()->m_charactersMeshResource.lock()->ForceLoad();
    rendererComponent.lock()->m_charactersMeshResource.lock()->ReLoadVertices(quadVertices);
    rendererComponent.lock()->m_charactersMeshResource.lock()->m_isStatic = false;
    rendererComponent.lock()->m_objectsMeshResource = g_game->GetResourceManager().AddResource<MeshResource>("", "", false);
    rendererComponent.lock()->m_objectsMeshResource.lock()->ForceLoad();
    rendererComponent.lock()->m_objectsMeshResource.lock()->ReLoadVertices(quadVertices);
    rendererComponent.lock()->m_objectsMeshResource.lock()->m_isStatic = false;

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::PreRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::Render()
{
    OPTICK_EVENT();

    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();

    ResourceManager& resourceManager = g_game->GetResourceManager();
    std::weak_ptr<SpriteResource> mainMenuSpriteResource = resourceManager.GetResourceByFile<SpriteResource>(rendererComponent.lock()->m_mainMenuTextureName.c_str());
    if (!mainMenuSpriteResource.expired()) {
        rendererComponent.lock()->DrawBackgroundTexturedQuad(mainMenuSpriteResource.lock()->GetTexture());
    }
    std::weak_ptr<SpriteResource> scoreboardSpriteResource = resourceManager.GetResourceByFile<SpriteResource>(rendererComponent.lock()->m_scoreboardTextureName.c_str());
    if (!scoreboardSpriteResource.expired()) {
        rendererComponent.lock()->DrawBackgroundTexturedQuad(scoreboardSpriteResource.lock()->GetTexture());
    }
    std::weak_ptr<SpriteResource> mapSpriteResource = resourceManager.GetResourceByFile<SpriteResource>("map.png");
    if (!mapSpriteResource.expired()) {
        rendererComponent.lock()->DrawMapTexturedQuad(mapSpriteResource.lock()->GetTexture());
    }
    std::weak_ptr<SpriteResource> charactersSpriteResource = resourceManager.GetResourceByFile<SpriteResource>("characters.png");
    if (!charactersSpriteResource.expired()) {
        rendererComponent.lock()->DrawCharactersTexturedQuad(charactersSpriteResource.lock()->GetTexture());
    }
    std::weak_ptr<SpriteResource> objectsSpriteResource = resourceManager.GetResourceByFile<SpriteResource>("objects.png");
    if (!objectsSpriteResource.expired()) {
        rendererComponent.lock()->DrawObjectsTexturedQuad(objectsSpriteResource.lock()->GetTexture());
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::PostRender()
{
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    if (windowComponent.lock()->m_isResized) {
        windowComponent.lock()->UpdateResolution();
        windowComponent.lock()->m_isResized = false;
    }

    SDL_GL_SwapWindow(windowComponent.lock()->m_window);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RendererSystem::ShutDown()
{
    SDL_GL_DeleteContext(SDL_GL_GetCurrentContext());

    return true;
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::SYSTEM_ENTITY_ADDED: {
        if (event.system.systemType == GetType()) {
            OnSystemEntityAdded(event.system.entity);
        }
        break;
    }

    case EventType::SYSTEM_ENTITY_REMOVED: {
        if (event.system.systemType == GetType()) {
            OnSystemEntityRemoved(event.system.entity);
        }
        break;
    }

    case EventType::WINDOW_RESIZED: {
        OnWindowResized();
        break;
    }

    case EventType::COMPONENT_MEMBER_CHANGED: {
        OnComponentMemberChanged(event.component.dirtyState, event.component.entity);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::OnSystemEntityAdded(Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
    if (spriteComponent.expired() || spriteComponent.lock()->m_spriteResource.expired()) {
        RecalculateAllMeshes();
        return;
    }

    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();

    const std::string textureFile = spriteComponent.lock()->m_spriteResource.lock()->GetFile();
    if (textureFile == rendererComponent.lock()->m_mainMenuTextureName.c_str() || textureFile == rendererComponent.lock()->m_scoreboardTextureName.c_str()) {
        RecalculateBackgroundMesh();
    } else if (textureFile == "map.png") {
        RecalculateMapMesh();
    } else if (textureFile == "characters.png") {
        RecalculateCharactersMesh();
    } else if (textureFile == "objects.png") {
        RecalculateObjectsMesh();
    }
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::OnSystemEntityRemoved(Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
    if (spriteComponent.expired() || spriteComponent.lock()->m_spriteResource.expired()) {
        RecalculateAllMeshes();
        return;
    }

    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();

    const std::string textureFile = spriteComponent.lock()->m_spriteResource.lock()->GetFile();
    if (textureFile == rendererComponent.lock()->m_mainMenuTextureName.c_str() || textureFile == rendererComponent.lock()->m_scoreboardTextureName.c_str()) {
        RecalculateBackgroundMesh();
    } else if (textureFile == "map.png") {
        RecalculateMapMesh();
    } else if (textureFile == "characters.png") {
        RecalculateCharactersMesh();
    } else if (textureFile == "objects.png") {
        RecalculateObjectsMesh();
    }
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::OnWindowResized() const
{
    RecalculateAllMeshes();
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::OnComponentMemberChanged(U64 dirtyState, Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    std::vector<Entity>::const_iterator it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it == m_entities.end()) {
        return;
    }

    std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
    if (spriteComponent.lock()->m_spriteResource.expired()) {
        return;
    }

    const bool hasPosition = dirtyState & static_cast<U64>(ComponentMemberType::TRANSFORM_POSITION);
    const bool hasRotation = dirtyState & static_cast<U64>(ComponentMemberType::TRANSFORM_ROTATION);
    const bool hasScale = dirtyState & static_cast<U64>(ComponentMemberType::TRANSFORM_SCALE);
    const bool hasSpriteNameToTextureCoords = dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS);
    const bool hasSpriteName = dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME);
    const bool isFlippedHorizontally = dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_FLIPPED_HORIZONTALLY);
    const bool isFlippedVertically = dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_FLIPPED_VERTICALLY);
    const bool hasColor = dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_COLOR);
    const bool hasPct = dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_PCT);
    if (!hasPosition && !hasRotation && !hasScale && !hasSpriteNameToTextureCoords && !hasSpriteName && !isFlippedHorizontally && !isFlippedVertically && !hasColor && !hasPct) {
        return;
    }

    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();

    const std::string textureFile = spriteComponent.lock()->m_spriteResource.lock()->GetFile();
    if (textureFile == rendererComponent.lock()->m_mainMenuTextureName.c_str() || textureFile == rendererComponent.lock()->m_scoreboardTextureName.c_str()) {
        RecalculateBackgroundMesh();
    } else if (textureFile == "map.png") {
        RecalculateMapMesh();
    } else if (textureFile == "characters.png") {
        RecalculateCharactersMesh();
    } else if (textureFile == "objects.png") {
        RecalculateObjectsMesh();
    }
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::RecalculateBackgroundMesh() const
{
    OPTICK_EVENT();

    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.lock()->GetProportion();

    std::vector<MeshResource::Instance> instances;
    for (const auto& entity : m_entities) {
        std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
        if (spriteComponent.lock()->m_spriteResource.expired()) {
            continue;
        }

        const std::string textureFile = spriteComponent.lock()->m_spriteResource.lock()->GetFile();
        if (textureFile != rendererComponent.lock()->m_mainMenuTextureName.c_str() && textureFile != rendererComponent.lock()->m_scoreboardTextureName.c_str()) {
            continue;
        }

        MeshResource::Instance instance;

        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        glm::vec3 position = transformComponent.lock()->GetPositionAndLayer();
        F32 rotation = transformComponent.lock()->m_rotation;
        glm::uvec4 textureCoords = spriteComponent.lock()->GetSpriteTextureCoords();
        glm::vec3 scale = glm::vec3(static_cast<F32>(textureCoords.z), static_cast<F32>(textureCoords.w), 0.0f);
        scale *= transformComponent.lock()->m_scale;
        if (spriteComponent.lock()->m_isFlippedHorizontally) {
            scale.x *= -1.0f;
        }
        if (spriteComponent.lock()->m_isFlippedVertically) {
            scale.y *= -1.0f;
        }

        glm::mat4 model = glm::mat4(1.0f);
        position.x *= proportion.x;
        position.y *= proportion.y;
        position.y *= -1.0f;
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, -1.0f));
        scale.x *= proportion.x;
        scale.y *= proportion.y;
        model = glm::scale(model, scale);
        instance.m_model = model;

        glm::uvec2 textureSize = spriteComponent.lock()->m_spriteResource.lock()->GetSize();
        glm::vec2 texCoords0 = glm::vec2(textureCoords.x / static_cast<F32>(textureSize.x), 1.0f - textureCoords.y / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[0] = texCoords0;
        glm::vec2 texCoords1 = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(textureSize.x), 1.0f - textureCoords.y / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[1] = texCoords1;
        glm::vec2 texCoords2 = glm::vec2(textureCoords.x / static_cast<F32>(textureSize.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[2] = texCoords2;
        glm::vec2 texCoords3 = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(textureSize.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[3] = texCoords3;

        instance.m_color = spriteComponent.lock()->m_color;
        instance.m_pct = spriteComponent.lock()->m_pct;

        instances.emplace_back(instance);
    }

    rendererComponent.lock()->m_backgroundMeshResource.lock()->ReLoadInstances(instances);
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::RecalculateMapMesh() const
{
    OPTICK_EVENT();

    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.lock()->GetProportion();

    std::vector<MeshResource::Instance> instances;
    for (const auto& entity : m_entities) {
        std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
        if (spriteComponent.lock()->m_spriteResource.expired()) {
            continue;
        }

        const std::string textureFile = spriteComponent.lock()->m_spriteResource.lock()->GetFile();
        if (textureFile != "map.png") {
            continue;
        }

        MeshResource::Instance instance;

        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        glm::vec3 position = transformComponent.lock()->GetPositionAndLayer();
        F32 rotation = transformComponent.lock()->m_rotation;
        glm::uvec4 textureCoords = spriteComponent.lock()->GetSpriteTextureCoords();
        glm::vec3 scale = glm::vec3(static_cast<F32>(textureCoords.z), static_cast<F32>(textureCoords.w), 0.0f);
        scale *= transformComponent.lock()->m_scale;
        if (spriteComponent.lock()->m_isFlippedHorizontally) {
            scale.x *= -1.0f;
        }
        if (spriteComponent.lock()->m_isFlippedVertically) {
            scale.y *= -1.0f;
        }

        glm::mat4 model = glm::mat4(1.0f);
        position.x *= proportion.x;
        position.y *= proportion.y;
        position.y *= -1.0f;
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, -1.0f));
        scale.x *= proportion.x;
        scale.y *= proportion.y;
        model = glm::scale(model, scale);
        instance.m_model = model;

        glm::uvec2 textureSize = spriteComponent.lock()->m_spriteResource.lock()->GetSize();
        glm::vec2 texCoords0 = glm::vec2(textureCoords.x / static_cast<F32>(textureSize.x), 1.0f - textureCoords.y / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[0] = texCoords0;
        glm::vec2 texCoords1 = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(textureSize.x), 1.0f - textureCoords.y / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[1] = texCoords1;
        glm::vec2 texCoords2 = glm::vec2(textureCoords.x / static_cast<F32>(textureSize.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[2] = texCoords2;
        glm::vec2 texCoords3 = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(textureSize.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[3] = texCoords3;

        instance.m_color = spriteComponent.lock()->m_color;
        instance.m_pct = spriteComponent.lock()->m_pct;

        instances.emplace_back(instance);
    }

    rendererComponent.lock()->m_mapMeshResource.lock()->ReLoadInstances(instances);
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::RecalculateCharactersMesh() const
{
    OPTICK_EVENT();

    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.lock()->GetProportion();

    std::vector<MeshResource::Instance> instances;
    for (const auto& entity : m_entities) {
        std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
        if (spriteComponent.lock()->m_spriteResource.expired()) {
            continue;
        }

        const std::string textureFile = spriteComponent.lock()->m_spriteResource.lock()->GetFile();
        if (textureFile != "characters.png") {
            continue;
        }

        MeshResource::Instance instance;

        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        glm::vec3 position = transformComponent.lock()->GetPositionAndLayer();
        F32 rotation = transformComponent.lock()->m_rotation;
        glm::uvec4 textureCoords = spriteComponent.lock()->GetSpriteTextureCoords();
        glm::vec3 scale = glm::vec3(static_cast<F32>(textureCoords.z), static_cast<F32>(textureCoords.w), 0.0f);
        scale *= transformComponent.lock()->m_scale;
        if (spriteComponent.lock()->m_isFlippedHorizontally) {
            scale.x *= -1.0f;
        }
        if (spriteComponent.lock()->m_isFlippedVertically) {
            scale.y *= -1.0f;
        }

        glm::mat4 model = glm::mat4(1.0f);
        position.x *= proportion.x;
        position.y *= proportion.y;
        position.y *= -1.0f;
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, -1.0f));
        scale.x *= proportion.x;
        scale.y *= proportion.y;
        model = glm::scale(model, scale);
        instance.m_model = model;

        glm::uvec2 textureSize = spriteComponent.lock()->m_spriteResource.lock()->GetSize();
        glm::vec2 texCoords0 = glm::vec2(textureCoords.x / static_cast<F32>(textureSize.x), 1.0f - textureCoords.y / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[0] = texCoords0;
        glm::vec2 texCoords1 = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(textureSize.x), 1.0f - textureCoords.y / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[1] = texCoords1;
        glm::vec2 texCoords2 = glm::vec2(textureCoords.x / static_cast<F32>(textureSize.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[2] = texCoords2;
        glm::vec2 texCoords3 = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(textureSize.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[3] = texCoords3;

        instance.m_color = spriteComponent.lock()->m_color;
        instance.m_pct = spriteComponent.lock()->m_pct;

        instances.emplace_back(instance);
    }

    rendererComponent.lock()->m_charactersMeshResource.lock()->ReLoadInstances(instances);
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::RecalculateObjectsMesh() const
{
    OPTICK_EVENT();

    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.lock()->GetProportion();

    std::vector<MeshResource::Instance> instances;
    for (const auto& entity : m_entities) {
        std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
        if (spriteComponent.lock()->m_spriteResource.expired()) {
            continue;
        }

        const std::string textureFile = spriteComponent.lock()->m_spriteResource.lock()->GetFile();
        if (textureFile != "objects.png") {
            continue;
        }

        MeshResource::Instance instance;

        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        glm::vec3 position = transformComponent.lock()->GetPositionAndLayer();
        F32 rotation = transformComponent.lock()->m_rotation;
        glm::uvec4 textureCoords = spriteComponent.lock()->GetSpriteTextureCoords();
        glm::vec3 scale = glm::vec3(static_cast<F32>(textureCoords.z), static_cast<F32>(textureCoords.w), 0.0f);
        scale *= transformComponent.lock()->m_scale;
        if (spriteComponent.lock()->m_isFlippedHorizontally) {
            scale.x *= -1.0f;
        }
        if (spriteComponent.lock()->m_isFlippedVertically) {
            scale.y *= -1.0f;
        }

        glm::mat4 model = glm::mat4(1.0f);
        position.x *= proportion.x;
        position.y *= proportion.y;
        position.y *= -1.0f;
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, -1.0f));
        scale.x *= proportion.x;
        scale.y *= proportion.y;
        model = glm::scale(model, scale);
        instance.m_model = model;

        glm::uvec2 textureSize = spriteComponent.lock()->m_spriteResource.lock()->GetSize();
        glm::vec2 texCoords0 = glm::vec2(textureCoords.x / static_cast<F32>(textureSize.x), 1.0f - textureCoords.y / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[0] = texCoords0;
        glm::vec2 texCoords1 = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(textureSize.x), 1.0f - textureCoords.y / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[1] = texCoords1;
        glm::vec2 texCoords2 = glm::vec2(textureCoords.x / static_cast<F32>(textureSize.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[2] = texCoords2;
        glm::vec2 texCoords3 = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(textureSize.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[3] = texCoords3;

        instance.m_color = spriteComponent.lock()->m_color;
        instance.m_pct = spriteComponent.lock()->m_pct;

        instances.emplace_back(instance);
    }

    rendererComponent.lock()->m_objectsMeshResource.lock()->ReLoadInstances(instances);
}

//----------------------------------------------------------------------------------------------------
void RendererSystem::RecalculateAllMeshes() const
{
    OPTICK_EVENT();

    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.lock()->GetProportion();

    std::vector<MeshResource::Instance> backgroundInstances;
    std::vector<MeshResource::Instance> mapInstances;
    std::vector<MeshResource::Instance> charactersInstances;
    std::vector<MeshResource::Instance> objectsInstances;
    for (const auto& entity : m_entities) {
        std::weak_ptr<SpriteComponent> spriteComponent = g_game->GetComponentManager().GetComponent<SpriteComponent>(entity);
        if (spriteComponent.lock()->m_spriteResource.expired()) {
            continue;
        }

        MeshResource::Instance instance;

        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        glm::vec3 position = transformComponent.lock()->GetPositionAndLayer();
        F32 rotation = transformComponent.lock()->m_rotation;
        glm::uvec4 textureCoords = spriteComponent.lock()->GetSpriteTextureCoords();
        glm::vec3 scale = glm::vec3(static_cast<F32>(textureCoords.z), static_cast<F32>(textureCoords.w), 0.0f);
        scale *= transformComponent.lock()->m_scale;
        if (spriteComponent.lock()->m_isFlippedHorizontally) {
            scale.x *= -1.0f;
        }
        if (spriteComponent.lock()->m_isFlippedVertically) {
            scale.y *= -1.0f;
        }

        glm::mat4 model = glm::mat4(1.0f);
        position.x *= proportion.x;
        position.y *= proportion.y;
        position.y *= -1.0f;
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, -1.0f));
        scale.x *= proportion.x;
        scale.y *= proportion.y;
        model = glm::scale(model, scale);
        instance.m_model = model;

        glm::uvec2 textureSize = spriteComponent.lock()->m_spriteResource.lock()->GetSize();
        glm::vec2 texCoords0 = glm::vec2(textureCoords.x / static_cast<F32>(textureSize.x), 1.0f - textureCoords.y / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[0] = texCoords0;
        glm::vec2 texCoords1 = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(textureSize.x), 1.0f - textureCoords.y / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[1] = texCoords1;
        glm::vec2 texCoords2 = glm::vec2(textureCoords.x / static_cast<F32>(textureSize.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[2] = texCoords2;
        glm::vec2 texCoords3 = glm::vec2((textureCoords.x + textureCoords.z) / static_cast<F32>(textureSize.x), 1.0f - (textureCoords.y + textureCoords.w) / static_cast<F32>(textureSize.y));
        instance.m_spriteCoords[3] = texCoords3;

        instance.m_color = spriteComponent.lock()->m_color;
        instance.m_pct = spriteComponent.lock()->m_pct;

        const std::string textureFile = spriteComponent.lock()->m_spriteResource.lock()->GetFile();
        if (textureFile == rendererComponent.lock()->m_mainMenuTextureName.c_str() || textureFile == rendererComponent.lock()->m_scoreboardTextureName.c_str()) {
            backgroundInstances.emplace_back(instance);
        } else if (textureFile == "map.png") {
            mapInstances.emplace_back(instance);
        } else if (textureFile == "characters.png") {
            charactersInstances.emplace_back(instance);
        } else if (textureFile == "objects.png") {
            objectsInstances.emplace_back(instance);
        }
    }

    rendererComponent.lock()->m_backgroundMeshResource.lock()->ReLoadInstances(backgroundInstances);
    rendererComponent.lock()->m_mapMeshResource.lock()->ReLoadInstances(mapInstances);
    rendererComponent.lock()->m_charactersMeshResource.lock()->ReLoadInstances(charactersInstances);
    rendererComponent.lock()->m_objectsMeshResource.lock()->ReLoadInstances(objectsInstances);
}
}
