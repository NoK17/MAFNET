#pragma once
#include <stdint.h>
#include "NetAddress.h"

namespace MAFNET {
	class NetPacket {
	private:
		NetAddress remoteAddress; //both recipient and sender.
		uint8_t packetID; //this is serialized into a 2 bit number when sending.
	public:
		enum class Type {
			PACKET_EMPTY = 0,
			CLIENT_CONNREQUEST = 1,
			CLIENT_DISCONNECT = 2,
			CLIENT_STATE = 3,
			SERVER_CONNRESPONSE = 1,
			SERVER_CONNTERMINATION = 2,
			SERVER_STATE = 3
		};

		NetPacket(Type type, NetAddress remoteAddress) : packetID((uint8_t)type), remoteAddress(remoteAddress) {}

		NetAddress getRemoteAddress() const {
			return remoteAddress;
		}
	};
}