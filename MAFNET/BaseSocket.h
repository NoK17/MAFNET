#pragma once
#include <WinSock2.h>
#include "NetAddress.h"

namespace MAFNET {

	struct BaseSocket {
	private:
		std::shared_ptr<uint64_t> transmissionBuffer; //The current data to be sent or receieved!
	public:
		const SOCKET socket;
		const NetAddress localAddress;

		BaseSocket(const SOCKET socket = 0, const NetAddress localAddress = NetAddress("", -1), std::shared_ptr<uint64_t> transmissionBuffer = nullptr) : socket(socket), localAddress(localAddress), transmissionBuffer(transmissionBuffer) {}

		std::shared_ptr<uint64_t> getTransmissionBuffer() const {
			return transmissionBuffer;
		}

		bool isInvalid() const {
			return (localAddress.getPort() == -1) || (transmissionBuffer == nullptr);
		}
	};
}