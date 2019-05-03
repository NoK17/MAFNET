#pragma once
#include <string>
#include <WinSock2.h> //This will change in the future to make the API cross-compatible.
#include "RawSocket.h"

namespace MAFNET {
	constexpr uint16_t MAFNET_TRANSPORT_RAW_BUFFER_LENGTH = 1024;

	class TransportLayerRaw {
	private:
		bool initializedFlag;
	public:
		TransportLayerRaw() : initializedFlag(false) {}

		RawSocket createRawSocket(int16_t port = 0) {
			if (!initializedFlag) {
				WSAData wsData;
				if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
					return RawSocket();
				initializedFlag = true;
			}

			SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (sock == INVALID_SOCKET)
				return RawSocket();

			sockaddr_in addr;
			addr.sin_addr.s_addr = INADDR_ANY;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			memset(addr.sin_zero, 0, sizeof(CHAR) * 8);

			if (bind(sock, (sockaddr*)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
				return RawSocket();

			return RawSocket(sock, NetAddress("127.0.0.1", port));
		}

		void sendPacket(const RawSocket& socket, const NetPacket& netPacket) {
			char buffer[MAFNET_TRANSPORT_RAW_BUFFER_LENGTH];

			sockaddr_in remoteAddr;
			remoteAddr.sin_port = htons(netPacket.remoteAddress.getPort());
			remoteAddr.sin_addr.s_addr = inet_addr(netPacket.remoteAddress.getAddress());
			remoteAddr.sin_family = AF_INET;
			memset(remoteAddr.sin_zero, 0, sizeof(CHAR) * 8);

			if (sendto(socket.socket, buffer, MAFNET_TRANSPORT_RAW_BUFFER_LENGTH, 0, (sockaddr*)&remoteAddr, sizeof(sockaddr_in)) != MAFNET_TRANSPORT_RAW_BUFFER_LENGTH) {
				if (WSAGetLastError() == WSAECONNRESET) //We run UDP but windows says the target destination isnt't available anymore.
					return;
				throw std::string("An error has occured whilst sending data.."); //We will remake this later to something which can actually provide some information.
			}
		}

		bool receievePacket(const RawSocket& socket, NetPacket& netPacket) {//true if a packet is receieved, false otherwise.
			char buffer[MAFNET_TRANSPORT_RAW_BUFFER_LENGTH];
			sockaddr_in remoteAddr;

			int szRemoteAddr = sizeof(sockaddr_in);
			int bytesReceieved = recvfrom(socket.socket, buffer, MAFNET_TRANSPORT_RAW_BUFFER_LENGTH, 0, (sockaddr*)&remoteAddr, &szRemoteAddr);

			if (bytesReceieved == -1)
				throw std::string("An error has occured whilst receiving data.."); //We will remake this later to something which can actually provide some information.
			else if (bytesReceieved == MAFNET_TRANSPORT_RAW_BUFFER_LENGTH)
				return true;

			return false;
		}
	};
}