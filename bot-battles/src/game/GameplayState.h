#ifndef __GAMEPLAY_STATE_H__
#define __GAMEPLAY_STATE_H__

#include "State.h"

#include <memory>

namespace sand
{


	struct TransformComponent;

	//----------------------------------------------------------------------------------------------------
	class GameplayState : public State
	{
	public:
		GameplayState();
		~GameplayState() override;

		bool Create() override;
		bool Destroy() override;

		bool Enter() override;
		bool PreUpdate(F32 dt) override;
		bool Update(F32 dt) override;
		bool PostUpdate(F32 dt) override;
		bool Render() override;
		bool Exit() override;

	private:
		std::shared_ptr<TransformComponent> m_transform;
	};
}

#endif
