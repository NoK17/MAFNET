#pragma once
#include <WinSock2.h>
#include "NetAddress.h"

namespace MAFNET {

	struct BaseSocket {
	private:
		uint64_t* transmissionBuffer;
	public:
		const SOCKET socket;
		const NetAddress localAddress;

		BaseSocket(const SOCKET socket = 0, const NetAddress localAddress = NetAddress("", -1), uint64_t* transmissionBuffer = nullptr) : socket(socket), localAddress(localAddress), transmissionBuffer(transmissionBuffer) {}

		uint64_t* getTransmissionBuffer() {
			return transmissionBuffer;
		}

		bool isInvalid() {
			return (localAddress.getPort() == -1);
		}
	};
}