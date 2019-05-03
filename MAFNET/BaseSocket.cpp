#include "BaseSocket.h"

namespace MAFNET {
	BaseSocket::BaseSocket(const SOCKET socket, const NetAddress localAddress, std::shared_ptr<uint64_t> transmissionBuffer) : socket(socket), localAddress(localAddress), transmissionBuffer(transmissionBuffer) {}

	std::shared_ptr<uint64_t> MAFNET::BaseSocket::getTransmissionBuffer() const {
		return transmissionBuffer;
	}

	bool MAFNET::BaseSocket::isInvalid() const {
		return (localAddress.getPort() == -1) || (transmissionBuffer == nullptr);
	}
}