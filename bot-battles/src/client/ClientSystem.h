#ifndef __CLIENT_SYSTEM_H__
#define __CLIENT_SYSTEM_H__

#include "ComponentDefs.h"
#include "Memory.h"
#include "System.h"

namespace sand {

	//----------------------------------------------------------------------------------------------------
	class ClientSystem : public System {
	public:
		static SystemType GetType()
		{
			return SystemType::CLIENT;
		}

	public:
		ClientSystem();
		~ClientSystem() override;

		bool StartUp() override;
		bool Update(F32 dt) override;
		bool ShutDown() override;

	private:
		void LogError(const char* message) const;
	};
}

#endif