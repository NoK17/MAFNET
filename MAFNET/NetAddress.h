#pragma once
#include <stdint.h>

namespace MAFNET {
	class NetAddress {
	private:
		const char* address;
		const int16_t port;
	public:
		NetAddress(const char* address = "", int16_t port = -1) : address(address), port(port) {}

		const char* getAddress() const {
			return address;
		}

		const int16_t getPort() const {
			return port;
		}
	};
}