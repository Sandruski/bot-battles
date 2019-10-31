#ifndef __CLIENT_SYSTEM_H__
#define __CLIENT_SYSTEM_H__

#include "ComponentDefs.h"
#include "System.h"

namespace sand {

	struct SingletonClientComponent;
	class OutputMemoryStream;
	class InputMemoryStream;

	//----------------------------------------------------------------------------------------------------
	class ClientSystem : public System {
	public:
		static SystemType GetType()
		{
			return SystemType::CLIENT;
		}

	public:
		enum class PacketType
		{
			HELLO
		};

	public:
		ClientSystem();
		~ClientSystem() override;

		bool StartUp() override;
		bool Update() override;
		bool ShutDown() override;

	private:
		void UpdateSayHello(SingletonClientComponent& client) const;
		void UpdateSendInput(SingletonClientComponent& client) const;

		void SendHelloPacket(const SingletonClientComponent& client) const;
		void SendInputPacket(const SingletonClientComponent& client) const;
	
		void ReceiveWelcomePacket(SingletonClientComponent& client, InputMemoryStream& stream) const;

		void SendPacket(const SingletonClientComponent& client, const OutputMemoryStream& stream) const;
	};
}

#endif
