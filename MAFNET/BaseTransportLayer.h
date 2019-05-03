#pragma once
#include <WinSock2.h> //This will change in the future to make the API cross-compatible.
#include <vector>
#include <string>
#include "BaseSocket.h"
#include "BasePacket.h"
#include "BaseSerializer.h"

namespace MAFNET {
	constexpr uint16_t TRANSPORT_RAW_BUFFER_BYTESZ = 1024;

	class BaseTransportLayer {
	private:
		bool initializedFlag;
	public:
		BaseTransportLayer() : initializedFlag(false) {}

		BaseSocket createRawSocket(int16_t port = 0) {
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

		bool sendPacket(const BaseSocket& socket, const BasePacket& netPacket) {
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
			uint64_t hash64 = 0; //TODO: gen hash for the entire transmit buffer right here, and serialize it into the start of the packet.
			socket.getTransmissionBuffer().get()[0] = hash64; //We do it this way because it is faster..

			if (sendto(socket.socket, (char*)socket.getTransmissionBuffer().get(), TRANSPORT_RAW_BUFFER_BYTESZ, 0, (sockaddr*)&remoteAddr, sizeof(sockaddr_in)) != TRANSPORT_RAW_BUFFER_BYTESZ) {
				if (WSAGetLastError() == WSAECONNRESET) //We run UDP but windows says the target destination isnt't available anymore.
					return true;
				return false;
			}
			return true;
		}

		bool receievePacket(const BaseSocket& socket, BasePacket& netPacket) {//false if an error occurred, true means that no error occurred.
			sockaddr_in remoteAddr;
			int szRemoteAddr = sizeof(sockaddr_in);

			memset(socket.getTransmissionBuffer().get(), 0, TRANSPORT_RAW_BUFFER_BYTESZ);
			int bytesReceieved = recvfrom(socket.socket, (char*)socket.getTransmissionBuffer().get(), TRANSPORT_RAW_BUFFER_BYTESZ, 0, (sockaddr*)&remoteAddr, &szRemoteAddr);

			if (bytesReceieved == -1)
				return false;
			
			netPacket = BasePacket(BasePacket::ID::PACKET_EMPTY, 0, NetAddress());

			if (bytesReceieved == TRANSPORT_RAW_BUFFER_BYTESZ) {
				BitUnpacker packer(socket.getTransmissionBuffer().get(), TRANSPORT_RAW_BUFFER_BYTESZ / sizeof(uint64_t));
				uint64_t packetID;
				uint64_t claimedHash = socket.getTransmissionBuffer().get()[0]; //What the packet claims that the transmit buffer hash should be.

				socket.getTransmissionBuffer().get()[0] = 0;

				//TODO: What has to happen here is that we hash the entire transmit buffer, then compare the hash from the packet to this.
				uint64_t transmitBufferHash = 0;

				if (claimedHash != transmitBufferHash) { //The packet was changed from the original.
					return true;
				}

				packer.setPackerPosition(1, 0); //set the packer to its position in the transmit buffer, and serialize the 2 bit id value.
				if (!packer.serializeUint64(packetID, 0, 3))
					return true;

				netPacket = BasePacket((BasePacket::ID) packetID, claimedHash, NetAddress(inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port)));
			}

			return true;
		}
	};
}