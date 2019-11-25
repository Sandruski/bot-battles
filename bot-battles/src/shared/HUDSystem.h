#ifndef __HUD_SYSTEM_H__
#define __HUD_SYSTEM_H__

#include "System.h"

namespace sand {

	//----------------------------------------------------------------------------------------------------
	class HUDSystem : public System {
	public:
		static SystemType GetType()
		{
			return SystemType::HUD;
		}

	public:
		HUDSystem();
		~HUDSystem() override;

		bool StartUp() override;
		bool Render() override;
		bool ShutDown() override;
	};
}

#endif
