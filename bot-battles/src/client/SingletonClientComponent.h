#ifndef __SINGLETON_CLIENT_COMPONENT_H__
#define __SINGLETON_CLIENT_COMPONENT_H__

#include "ComponentDefs.h"


namespace sand {


	class SocketAddress;
	class UDPSocket;

	//----------------------------------------------------------------------------------------------------
	struct SingletonClientComponent {
		static SingletonComponentType GetType() { return SingletonComponentType::CLIENT; }

		SingletonClientComponent()
			: m_socket(nullptr),
			m_socketAddress(nullptr)
		{		
		}
		~SingletonClientComponent() { }

		std::shared_ptr<UDPSocket> m_socket;
		std::shared_ptr<SocketAddress> m_socketAddress;
	};
}

#endif
