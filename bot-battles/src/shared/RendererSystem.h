#ifndef __RENDERER_SYSTEM_H__
#define __RENDERER_SYSTEM_H__

#include "System.h"

namespace sand {

struct SingletonRendererComponent;

//----------------------------------------------------------------------------------------------------
class RendererSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::RENDERER;
    }

public:
    RendererSystem();
    ~RendererSystem() override;

    bool StartUp() override;
	bool PreRender() override;
    bool Render() override;
	bool PostRender() override;
    bool ShutDown() override;
};
}

#endif
