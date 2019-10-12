#ifndef __GAMEPLAY_STATE_H__
#define __GAMEPLAY_STATE_H__

#include "State.h"

#include <memory>

namespace sand
{

	class ResourceTexture;

	//----------------------------------------------------------------------------------------------------
	class GameplayState : public State
	{
	public:
		GameplayState();
		~GameplayState() override;

		bool Create() override;
		bool Destroy() override;

		bool Enter() override;
		bool Update(F32 dt) override;
		bool LateUpdate(F32 dt) override;
		bool Draw() override;
		bool Exit() override;
	};
}

#endif
