#pragma once
#include <stdint.h>

namespace MAFNET {
	class NetAddress {
	private:
		const char* address;
		int16_t port;
	public:
		NetAddress(const char* address = "", int16_t port = -1);
		const char* getAddress() const;
		const int16_t getPort() const;
	};
}