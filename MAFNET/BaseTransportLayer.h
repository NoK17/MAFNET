#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h> //This will change in the future to make the API cross-compatible.
#include <vector>
#include <string>
#include "BaseSocket.h"
#include "BasePacket.h"
#include "BaseSerializer.h"

namespace MAFNET {
	constexpr uint16_t TRANSPORT_RAW_BUFFER_BYTESZ = 1024;

	class BaseTransportLayer {
	private:
		bool initializedFlag;
	public:
		BaseTransportLayer();
		BaseSocket createRawSocket(int16_t port = 0);
		bool sendPacket(const BaseSocket& socket, const BasePacket& netPacket);
		bool receievePacket(const BaseSocket& socket, BasePacket& netPacket);
	};
}