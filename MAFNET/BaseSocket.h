#pragma once
#include <WinSock2.h>
#include "NetAddress.h"
#include "NetPacket.h"

namespace MAFNET {
	struct BaseSocket {
		const SOCKET socket;
		const NetAddress localAddress;

		BaseSocket(const SOCKET socket = 0, const NetAddress localAddress = NetAddress("", -1)) : socket(socket), localAddress(localAddress) {}

		bool isInvalid() {
			return (localAddress.getPort() == -1);
		}
	};
}