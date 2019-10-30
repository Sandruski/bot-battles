#ifndef __SINGLETON_SERVER_COMPONENT_H__
#define __SINGLETON_SERVER_COMPONENT_H__

#include "ComponentDefs.h"

#include "Memory.h"

#include <WinSock2.h>

namespace sand {

	//----------------------------------------------------------------------------------------------------
	struct SingletonServerComponent {
		static SingletonComponentType GetType() { return SingletonComponentType::SERVER; }

		SingletonServerComponent()
			: m_socket(NULL),
			m_address("127.0.0.1"),
			m_port(9999)
		{
		}
		~SingletonServerComponent() { }

		SOCKET m_socket;
		const char* m_address;
		U16 m_port;
	};
}

#endif