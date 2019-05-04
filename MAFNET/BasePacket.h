#pragma once
#include <stdint.h>
#include "NetAddress.h"

namespace MAFNET {
	class BasePacket {
		NetAddress remoteAddress; //both recipient and sender.
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

		BasePacket(BasePacket::ID id = BasePacket::ID::PACKET_EMPTY, NetAddress remoteAddress = NetAddress());

		const BasePacket::ID getPacketID() const;

		const bool isEmpty() const;

		NetAddress getRemoteAddress() const;
	};
}