#ifndef __SERVER_SYSTEM_H__
#define __SERVER_SYSTEM_H__

#include "ComponentDefs.h"
#include "Memory.h"
#include "System.h"

namespace sand {

	//----------------------------------------------------------------------------------------------------
	class ServerSystem : public System {
	public:
		static SystemType GetType()
		{
			return SystemType::SERVER;
		}

	public:
		ServerSystem();
		~ServerSystem() override;

		bool StartUp() override;
		bool Update(F32 dt) override;
		bool ShutDown() override;

	private:
		void LogError(const char* message) const;
	};
}

#endif