#pragma once
#include "NetAddress.h"
#include "NetPacket.h"

namespace MAFNET {
	struct RawSocket {
		const SOCKET socket;
		const NetAddress localAddress;

		RawSocket(const SOCKET socket = 0, const NetAddress localAddress = NetAddress("", -1)) : socket(socket), localAddress(localAddress) {}

		bool isInvalid() {
			return (localAddress.getPort() == -1);
		}
	};
}