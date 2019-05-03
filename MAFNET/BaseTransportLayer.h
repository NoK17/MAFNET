#pragma once
#include <vector>
#include <string>
#include <WinSock2.h> //This will change in the future to make the API cross-compatible.
#include "BaseSocket.h"
#include "BasePacket.h"

namespace MAFNET {
	constexpr uint16_t TRANSPORT_RAW_BUFFER_BYTESZ = 1024;

	class BaseTransportLayer {
	private:
		bool initializedFlag;
		std::vector<uint64_t[TRANSPORT_RAW_BUFFER_BYTESZ / sizeof(uint64_t)]> transmissionBuffers;
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

			transmissionBuffers.resize(transmissionBuffers.size() + 1);
			auto& transmissionBuffer = transmissionBuffers[transmissionBuffers.size() - 1];

			memset(transmissionBuffer, 0, TRANSPORT_RAW_BUFFER_BYTESZ);
			return BaseSocket(sock, NetAddress("127.0.0.1", port), transmissionBuffer);
		}

		bool sendPacket(const BaseSocket& socket, const BasePacket& netPacket) {
			uint64_t buffer[TRANSPORT_RAW_BUFFER_BYTESZ / sizeof(uint64_t)];

			sockaddr_in remoteAddr;
			remoteAddr.sin_port = htons(netPacket.getRemoteAddress().getPort());
			remoteAddr.sin_addr.s_addr = inet_addr(netPacket.getRemoteAddress().getAddress());
			remoteAddr.sin_family = AF_INET;
			memset(remoteAddr.sin_zero, 0, sizeof(CHAR) * 8);

			if (sendto(socket.socket, (char*) buffer, TRANSPORT_RAW_BUFFER_BYTESZ, 0, (sockaddr*)&remoteAddr, sizeof(sockaddr_in)) != TRANSPORT_RAW_BUFFER_BYTESZ) {
				if (WSAGetLastError() == WSAECONNRESET) //We run UDP but windows says the target destination isnt't available anymore.
					return true;
				return false;
			}
			return true;
		}

		bool receievePacket(const BaseSocket& socket, BasePacket& netPacket) {//false if an error occurred, true means that no error occurred.
			uint64_t buffer[TRANSPORT_RAW_BUFFER_BYTESZ / sizeof(uint64_t)];
			sockaddr_in remoteAddr;

			int szRemoteAddr = sizeof(sockaddr_in);
			int bytesReceieved = recvfrom(socket.socket, (char*)buffer, TRANSPORT_RAW_BUFFER_BYTESZ, 0, (sockaddr*)&remoteAddr, &szRemoteAddr);

			if (bytesReceieved == -1)
				return false;
			
			netPacket = BasePacket(BasePacket::ID::PACKET_EMPTY, NetAddress(inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port)));

			if (bytesReceieved == TRANSPORT_RAW_BUFFER_BYTESZ) {
				//serialize the packet header here.
			}

			return true;
		}
	};
}