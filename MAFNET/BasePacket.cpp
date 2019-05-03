#include "BasePacket.h"

namespace MAFNET {
	BasePacket::BasePacket(BasePacket::ID id, uint64_t hash64, NetAddress remoteAddress) : packetID((uint8_t)id), hash64(hash64), remoteAddress(remoteAddress) {}

	const uint32_t MAFNET::BasePacket::getHash64() const {
		return hash64;
	}

	const BasePacket::ID BasePacket::getPacketID() const {
		return (BasePacket::ID) packetID;
	}

	NetAddress BasePacket::getRemoteAddress() const {
		return remoteAddress;
	}
}
