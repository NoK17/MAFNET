#include "NetAddress.h"

namespace MAFNET {
	NetAddress::NetAddress(const char* address, int16_t port) : address(address), port(port) {}

	const char* NetAddress::getAddress() const {
		return address;
	}

	const int16_t NetAddress::getPort() const {
		return port;
	}
}