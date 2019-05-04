#include "BasePacket.h"

namespace MAFNET {
	BasePacket::BasePacket(BasePacket::ID id, NetAddress remoteAddress) : packetID((uint8_t)id), remoteAddress(remoteAddress) {}

	const BasePacket::ID BasePacket::getPacketID() const {
		return (BasePacket::ID) packetID;
	}

	const bool BasePacket::isEmpty() const {
		return (((BasePacket::ID) packetID) == BasePacket::ID::PACKET_EMPTY);
	}

	NetAddress BasePacket::getRemoteAddress() const {
		return remoteAddress;
	}
}
