#include "BaseTransportLayer.h"

namespace MAFNET {
	BaseTransportLayer::BaseTransportLayer() : initializedFlag(false) {}

	BaseSocket BaseTransportLayer::createRawSocket(int16_t port) {
		if (!initializedFlag) {
			WSAData wsData;
			if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
				return BaseSocket();
			initializedFlag = true;
		}

		SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock == INVALID_SOCKET)
			return BaseSocket();

		sockaddr_in addr;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		memset(addr.sin_zero, 0, sizeof(CHAR) * 8);

		if (bind(sock, (sockaddr*)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
			return BaseSocket();

		u_long flag = 666;
		if (ioctlsocket(sock, FIONBIO, &flag) == SOCKET_ERROR)
			return BaseSocket();

		std::shared_ptr<uint64_t> transmissionBuffer = std::shared_ptr<uint64_t>(new uint64_t[TRANSPORT_RAW_BUFFER_BYTESZ / sizeof(uint64_t)]);

		memset(transmissionBuffer.get(), 0, TRANSPORT_RAW_BUFFER_BYTESZ);
		return BaseSocket(sock, NetAddress("127.0.0.1", port), transmissionBuffer);
	}

	bool BaseTransportLayer::sendPacket(const BaseSocket & socket, const BasePacket & netPacket) {
		sockaddr_in remoteAddr;
		remoteAddr.sin_port = htons(netPacket.getRemoteAddress().getPort());
		remoteAddr.sin_addr.s_addr = inet_addr(netPacket.getRemoteAddress().getAddress());
		remoteAddr.sin_family = AF_INET;
		memset(remoteAddr.sin_zero, 0, sizeof(CHAR) * 8);

		//serialize the basePacket here.
		memset(socket.getTransmissionBuffer().get(), 0, TRANSPORT_RAW_BUFFER_BYTESZ / sizeof(uint64_t));
		BitPacker packer(socket.getTransmissionBuffer().get(), TRANSPORT_RAW_BUFFER_BYTESZ / sizeof(uint64_t));


		//generate the hash for the packet data, which is set before this is called, since the expose bitpacker is offset to 66 bits.
		packer.setPackerPosition(1, 0);
		if (!packer.serializeUint64((uint64_t)netPacket.getPacketID(), 0, 3))
			return false;

		socket.getTransmissionBuffer().get()[0] = 0;
		uint64_t hash64 = XXH64(socket.getTransmissionBuffer().get(), TRANSPORT_RAW_BUFFER_BYTESZ, 42); //TODO: gen hash for the entire transmit buffer right here, and serialize it into the start of the packet.
		socket.getTransmissionBuffer().get()[0] = hash64; //We do it this way because it is faster..

		if (sendto(socket.socket, (char*)socket.getTransmissionBuffer().get(), TRANSPORT_RAW_BUFFER_BYTESZ, 0, (sockaddr*)&remoteAddr, sizeof(sockaddr_in)) != TRANSPORT_RAW_BUFFER_BYTESZ) {
			if (WSAGetLastError() == WSAECONNRESET) //We run UDP but windows says the target destination isnt't available anymore.
				return true;
			return false;
		}
		return true;
	}

	bool BaseTransportLayer::receievePacket(const BaseSocket & socket, BasePacket & netPacket) {//false if an error occurred, true means that no error occurred.
		sockaddr_in remoteAddr;
		int szRemoteAddr = sizeof(sockaddr_in);

		memset(socket.getTransmissionBuffer().get(), 0, TRANSPORT_RAW_BUFFER_BYTESZ);
		int bytesReceieved = recvfrom(socket.socket, (char*)socket.getTransmissionBuffer().get(), TRANSPORT_RAW_BUFFER_BYTESZ, 0, (sockaddr*)&remoteAddr, &szRemoteAddr);

		if (bytesReceieved == -1) {
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				return true;
			return false;
		}

		netPacket = BasePacket(BasePacket::ID::PACKET_EMPTY, NetAddress());

		if (bytesReceieved == TRANSPORT_RAW_BUFFER_BYTESZ) {
			BitUnpacker packer(socket.getTransmissionBuffer().get(), TRANSPORT_RAW_BUFFER_BYTESZ / sizeof(uint64_t));
			uint64_t packetID;
			uint64_t claimedHash = socket.getTransmissionBuffer().get()[0]; //What the packet claims that the transmit buffer hash should be.

			socket.getTransmissionBuffer().get()[0] = 0;

			//Hash the entire transmit buffer, then compare the hash from the packet to this.. if unequal, the packet has been changed.
			uint64_t transmitBufferHash = XXH64(socket.getTransmissionBuffer().get(), TRANSPORT_RAW_BUFFER_BYTESZ, 42);
			if (claimedHash != transmitBufferHash) //The packet was changed from the original.
				return true;

			Debug::writeMessage(Debug::MessageGravity::ALL, "Hash of message was fine: %" PRId64 " == %" PRId64, claimedHash, transmitBufferHash);

			packer.setPackerPosition(1, 0); //set the packer to its position in the transmit buffer, and serialize the 2 bit id value :))
			if (!packer.serializeUint64(packetID, 0, 3))
				return true;

			//When the public serializer is to be exposed, the starting position should be 1, calculateBits(0, 3) [2] :))
			netPacket = BasePacket((BasePacket::ID) packetID, NetAddress(inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port)));
		}

		return true;
	}
}