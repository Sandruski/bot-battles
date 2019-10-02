#ifndef __GAMEPLAY_STATE_H__
#define __GAMEPLAY_STATE_H__

#include "State.h"

#include <memory>

namespace sand
{

	class ModuleFSM;
	class ResourceTexture;

	//----------------------------------------------------------------------------------------------------
	class GameplayState : public State
	{
	public:
		GameplayState(std::shared_ptr<ModuleFSM> fsm);
		~GameplayState() override;

		bool Create() override;
		bool Destroy() override;

		bool Enter() override;
		bool Update(F32 dt) override;
		bool LateUpdate(F32 dt) override;
		bool Draw() override;
		bool Exit() override;

	public:
		std::shared_ptr<ResourceTexture> m_resourceTexture;

	private:
		std::shared_ptr<ModuleFSM> m_fsm;
	};
}

#endif