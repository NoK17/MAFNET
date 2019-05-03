#pragma once
#include <stdint.h>

namespace MAFNET {
	struct NetPacket {
		enum class Type {
			CLIENT_CONNREQUEST = 0,
			CLIENT_DISCONNECT = 1,
			CLIENT_STATE = 2,
			SERVER_CONNRESPONSE = 0,
			SERVER_CONNTERMINATION = 1,
			SERVER_STATE = 2
		};
		uint8_t packetID; //this is serialized into a 2 bit number when sending.
	};
}