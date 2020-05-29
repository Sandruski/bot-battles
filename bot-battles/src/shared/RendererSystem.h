#ifndef __RENDERER_SYSTEM_H__
#define __RENDERER_SYSTEM_H__

#include "System.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class RendererSystem : public System, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::RENDERER;
    }

public:
    RendererSystem();

    bool StartUp() override;
    bool PreRender() override;
    bool Render() override;
    bool PostRender() override;
    bool ShutDown() override;

    void OnNotify(const Event& event) override;

private:
    void OnSystemEntityAdded(Entity entity) const;
    void OnSystemEntityRemoved(Entity entity) const;
    void OnComponentMemberChanged(U64 dirtyState, Entity entity) const;

private:
    void RecalculateMapMesh() const;
    void RecalculateCharactersMesh() const;
    void RecalculateAllMeshes() const;
};
}

#endif
