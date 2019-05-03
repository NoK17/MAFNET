#pragma once
#include <stdint.h>
#include "NetAddress.h"

namespace MAFNET {
	class BasePacket {
		NetAddress remoteAddress; //both recipient and sender.
		uint64_t hash64; //this is the has that ensures that the contents of the packet remain unchanged.
		uint8_t packetID; //this is serialized into a 2 bit number when sending.
		//Together these are 34 bits in, important since that is where the serializer will start reading and writing in the serializer.. on the 35th bit.
	public:
		enum class ID {
			PACKET_EMPTY = 0,
			CLIENT_CONNREQUEST = 1,
			CLIENT_DISCONNECT = 2,
			CLIENT_STATE = 3,
			SERVER_CONNRESPONSE = 1,
			SERVER_CONNTERMINATION = 2,
			SERVER_STATE = 3
		};

		BasePacket(BasePacket::ID id, uint64_t hash64, NetAddress remoteAddress);

		const uint64_t getHash64() const;
		const BasePacket::ID getPacketID() const;

		NetAddress getRemoteAddress() const;
	};
}