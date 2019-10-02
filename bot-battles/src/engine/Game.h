#ifndef __GAME_H__
#define __GAME_H__

#include "Module.h"

#include <memory>

namespace sand
{

	class ResourceTexture;

	//----------------------------------------------------------------------------------------------------
	class Game : public Module
	{
	public:
		Game();
		~Game();

		bool StartUp() override;
		bool ShutDown() override;

		const char* GetName() const override;

		bool Update();

	public:
		std::shared_ptr<ResourceTexture> m_resourceTexture;
	};
}

#endif