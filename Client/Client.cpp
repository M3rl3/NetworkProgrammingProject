#include "Client.h"

struct Header {
	uint32_t packetLength;
	uint32_t messageLength;
	uint32_t roomNameLength;
	uint32_t usrNameLength;
};

struct Content {
	std::string roomName;
	std::string message;
	std::string usrName;
};

struct Packet {
	Header header;
	Content content;
};

std::vector<uint8_t> CreatePacket(std::string usrName, std::string roomName, std::string message) {
	Packet pkt;
	pkt.content.roomName = roomName;
	pkt.content.message = message;
	pkt.content.usrName = usrName;

	pkt.header.roomNameLength = sizeof(pkt.content.roomName);
	pkt.header.messageLength = sizeof(pkt.content.message);
	pkt.header.usrNameLength = sizeof(pkt.content.usrName);

	pkt.header.packetLength = 
		sizeof(pkt.header.roomNameLength) + sizeof(pkt.header.messageLength) +
		sizeof(pkt.header.usrNameLength) + pkt.header.roomNameLength + 
		pkt.header.messageLength + pkt.header.usrNameLength; 

	Buffer buffer;

	buffer.WriteUInt32LE(pkt.header.packetLength);
	buffer.WriteUInt32LE(pkt.header.usrNameLength);
	buffer.WriteUInt32LE(pkt.header.roomNameLength);
	buffer.WriteUInt32LE(pkt.header.messageLength);

	buffer.WriteString(pkt.content.usrName);
	buffer.WriteString(pkt.content.roomName);
	buffer.WriteString(pkt.content.message);


	return buffer.m_Buffer;
}

int Client::Initialize() {

	WSADATA wsa;
	int result;

	std::cout << "Startup running...\n";
	result = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (result != 0) {
		std::cout << "\nExit with code " << result << "." << std::endl;
		return 1;
	}

	ZeroMemory(&g_ClientInfo.hints, sizeof(g_ClientInfo.hints));
	g_ClientInfo.hints.ai_family = AF_INET;
	g_ClientInfo.hints.ai_socktype = SOCK_STREAM;
	g_ClientInfo.hints.ai_protocol = IPPROTO_TCP;
	g_ClientInfo.hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(LOCAL_HOST, DEFAULT_PORT, &g_ClientInfo.hints, &g_ClientInfo.info);
	std::cout << "Getting address info...\n";
	if (result != 0) {
		std::cout << "\nExit with code " << result << "." << std::endl;
		WSACleanup();
		return 1;
	}

	std::cout << "Creating socket...\n";
	g_ClientInfo.sock = socket(g_ClientInfo.info->ai_family, g_ClientInfo.info->ai_socktype, g_ClientInfo.info->ai_protocol);
	if (g_ClientInfo.sock == INVALID_SOCKET) {
		std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ClientInfo.info);
		WSACleanup();
		return 1;
	}

	std::cout << "Connecting to the server...\n";
	result = connect(g_ClientInfo.sock, g_ClientInfo.info->ai_addr, g_ClientInfo.info->ai_addrlen);
	if (result == SOCKET_ERROR) {
		std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ClientInfo.info);
		closesocket(g_ClientInfo.sock);
		WSACleanup();
		return 1;
	}

	std::cout << "Connection established...\n";

	return 0;
}

int Client::I_O() {

	const int buflen = 256;
	char buf[buflen];

	std::string uName;
	std::string roomName;
	
	std::cout << "\nEnter username: ";
	getline(std::cin, uName);

	/*std::cout << "\nRoomAlpha\nRoomBeta";

	std::cout << "\nSelect the room you wanna join: ";
	getline(std::cin, roomName);*/

	bool dataSent = true;

	std::cout << "Sending message to server...\n";

	do {
		std::string input;
		std::ostringstream ss;
		
		std::cout << "> ";
		getline(std::cin, input);
		
		/*ch = _getch();
		input += ch;*/
		
		/*fflush(stdin);
		getchar();*/

		//std::vector<uint8_t> buffPacket;
		//buffPacket = CreatePacket(uName, roomName, input);
		
		ss << uName << " : " << input << " | " << Time();
		std::string stream = ss.str();

		Buffer myBuf(stream.length());
		myBuf.WriteString(stream);

		//Exits the loop if input buffer size is 0
		if (input.size() > 0) {
	
			int sendResult = send(g_ClientInfo.sock, stream.c_str(), stream.size() + 1, 0);
			//int sendResult = send(g_ClientInfo.sock, (const char*)&buffPacket[0], buffPacket.size(), 0);
			if (sendResult != SOCKET_ERROR)
			{
				//Reset the buffer to receive on it
				ZeroMemory(buf, buflen);
				int bytesReceived = recv(g_ClientInfo.sock, buf, buflen, 0);
				if (bytesReceived > 0)
				{
					std::cout << std::string(buf, 0, bytesReceived) << std::endl;
				}
			}
			else {
				std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
			}
		}
		else {
			dataSent = false;
		}
	} while (dataSent);

	return 0;
}

std::string Client::Time() {
	auto time = std::chrono::system_clock::now();
	std::time_t con = std::chrono::system_clock::to_time_t(time);
	std::string conTime = std::ctime(&con);
	return conTime;
}

void Client::ShutDown() {
	std::cout << "\nShutting Down...";
	freeaddrinfo(g_ClientInfo.info);
	closesocket(g_ClientInfo.sock);
	WSACleanup();
}