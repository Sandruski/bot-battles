#ifndef __SINGLETON_CLIENT_COMPONENT_H__
#define __SINGLETON_CLIENT_COMPONENT_H__

#include "ComponentDefs.h"

#include "Memory.h"

#include <WinSock2.h>

namespace sand {

	//----------------------------------------------------------------------------------------------------
	struct SingletonClientComponent {
		static SingletonComponentType GetType() { return SingletonComponentType::CLIENT; }

		SingletonClientComponent()
			: m_socket(NULL),
			toAddress(),
			m_port(9999)
		{
		}
		~SingletonClientComponent() { }

		SOCKET m_socket;
		SOCKADDR_IN toAddress;
		U16 m_port;
	};
}

#endif