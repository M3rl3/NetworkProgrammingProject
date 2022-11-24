#include "SelectServer.h"

#include "auth.pb.h"

SelectServer::SelectServer() {

}

SelectServer::~SelectServer() {

}

int SelectServer::Initialize() {

	WSADATA wsa;
	int result;

	std::cout << "Startup running . . . ";
	result = WSAStartup(MAKEWORD(2, 2), &wsa);

	FD_ZERO(&g_ServerInfo.socksReadyForReading);
	g_ServerInfo.clientsConnected = 0;

	if (result != 0) {
		std::cout << "\nExit with code " << result << "." << std::endl;
		return 1;
	}
	else {
		printf("Success!\n");
	}


	ZeroMemory(&g_ServerInfo.hints, sizeof(g_ServerInfo.hints));
	
	g_ServerInfo.hints.ai_family = AF_INET;
	g_ServerInfo.hints.ai_socktype = SOCK_STREAM;
	g_ServerInfo.hints.ai_protocol = IPPROTO_TCP;
	g_ServerInfo.hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(LOCAL_HOST, AUTH_PORT, &g_ServerInfo.hints, &g_ServerInfo.info);
	std::cout << "Getting address info . . . ";
	if (result != 0) {
		std::cout << "\nFetching address info exit with code " << result << "." << std::endl;
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	std::cout << "Creating socket . . . ";
	g_ServerInfo.authSock = socket(g_ServerInfo.info->ai_family, g_ServerInfo.info->ai_socktype, g_ServerInfo.info->ai_protocol);
	if (g_ServerInfo.authSock == INVALID_SOCKET) {
		std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ServerInfo.info);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	std::cout << "Connecting to the authentication server . . . ";
	result = connect(g_ServerInfo.authSock, g_ServerInfo.info->ai_addr, g_ServerInfo.info->ai_addrlen);
	if (result == SOCKET_ERROR) {
		std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ServerInfo.info);
		closesocket(g_ServerInfo.authSock);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
		printf("Connected to authentication server!\n");
	}

	DWORD NonBlock = 1;
	result = ioctlsocket(g_ServerInfo.authSock, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) {
		std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ServerInfo.info);
		closesocket(g_ServerInfo.authSock);
		WSACleanup();
		return 1;
	}

	std::cout << "\n";

	result = getaddrinfo(LOCAL_HOST, DEFAULT_PORT, &g_ServerInfo.hints, &g_ServerInfo.info);
	std::cout << "Getting address info . . . ";
	if (result != 0) {
		std::cout << "\nFetching address info exit with code " << result << "." << std::endl;
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}
	
	std::cout << "Creating socket . . . ";
	//g_ServerInfo.listenSock = socket(g_ServerInfo.info->ai_family, g_ServerInfo.info->ai_socktype, g_ServerInfo.info->ai_protocol);
	g_ServerInfo.listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (g_ServerInfo.listenSock == INVALID_SOCKET) {
		std::cout << "\nSocket creation exit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ServerInfo.info);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	std::cout << "Binding socket . . . ";
	result = bind(g_ServerInfo.listenSock, g_ServerInfo.info->ai_addr, g_ServerInfo.info->ai_addrlen);
	if (result == SOCKET_ERROR) {
		std::cout << "\nSocket bind exit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ServerInfo.info);
		closesocket(g_ServerInfo.listenSock);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	std::cout << "Listening on socket . . . ";
	result = listen(g_ServerInfo.listenSock, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		std::cout << "\nListening on socket exit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ServerInfo.info);
		closesocket(g_ServerInfo.listenSock);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	return 0;
}

int SelectServer::I_O() {

	struct timeval t_val;
	int selResult;
	t_val.tv_sec = 0;
	t_val.tv_usec = 500 * 1000;

	bool executing = true;
	std::cout << "Calling select . . . \n";
	
	while (executing) {
		
		FD_ZERO(&g_ServerInfo.socksReadyForReading);
		FD_SET(g_ServerInfo.listenSock, &g_ServerInfo.socksReadyForReading);

		FD_SET(g_ServerInfo.authSock, &g_ServerInfo.socksReadyForReading);
		
		for (int i = 0; i < g_ServerInfo.clients.size(); i++) {
			ClientInfo& client = g_ServerInfo.clients[i];
			if (client.connected) {
				FD_SET(client.cSock, &g_ServerInfo.socksReadyForReading);
			}	
		}

		selResult = select(0, &g_ServerInfo.socksReadyForReading, nullptr, nullptr, &t_val);
		if (selResult == SOCKET_ERROR) {
			FD_CLR(g_ServerInfo.clientSock, &g_ServerInfo.socksReadyForReading);
			FD_SET(g_ServerInfo.listenSock, &g_ServerInfo.socksReadyForReading);
		}

		std::cout << ".";

		/*if (FD_ISSET(g_ServerInfo.authSock, &g_ServerInfo.socksReadyForReading)) {
			std::string yes;
			yes = "yes";
			int result = send(g_ServerInfo.authSock, yes.c_str(), yes.size(), 0);
			if (result == SOCKET_ERROR) {
				std::cout << "Could not send buffer." << std::endl;
			}
			const int buflen = 256;
			char buf[buflen];
			ZeroMemory(buf, buflen);
			int recvResult = recv(g_ServerInfo.authSock, buf, buflen, 0);
			std::cout << "Message from auth server" << buf << std::endl;
		}*/

		// Inbound connection
		if (FD_ISSET(g_ServerInfo.listenSock, &g_ServerInfo.socksReadyForReading)) {
			
			SOCKET clientSocket = accept(g_ServerInfo.listenSock, nullptr, nullptr);
			if (clientSocket == INVALID_SOCKET) {
				std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
			}
			else {
				ClientInfo client;
				client.buffer = Buffer(256);
				client.connected = true;
				client.cSock = clientSocket;
				FD_SET(client.cSock, &g_ServerInfo.socksReadyForReading);
				
				std::string welMessage = "Welcome to the chat server!";

				// Serialize the welcome message too!
				Buffer buffer(welMessage.length());
				welMessage = buffer.WriteString(welMessage, 0);

				send(client.cSock, welMessage.c_str(), welMessage.size() + 1, 0);

				g_ServerInfo.clients.push_back(client);
			}
		}

		// Inbound message
		for (int i = 0; i < g_ServerInfo.clients.size(); i++) {
			
			ClientInfo client = g_ServerInfo.clients[i];
			
			if (client.connected == false) {
				continue;
			}

			if (FD_ISSET(client.cSock, &g_ServerInfo.socksReadyForReading)) {
				const int buflen = 256;
				char buf[buflen];
				//ZeroMemory(buf, buflen);

				// Receive buffer
				//Buffer buf1 = Buffer(128);
				//int bytesReceived = recv(client.cSock,(char*)&(buf1.m_Buffer[0]), buflen, 0);
				int bytesReceived = Receive(client, buflen, buf);
				// Buffer before being deserialized.
				std::cout << "\n" << buf;

				//uint32_t temp = 0;
				//uint32_t packetLen = buf1.ReadUInt32LE(temp);

				//std::vector<uint8_t> hello;

				////buf1.m_Buffer(128);
				// std::string data = buf;

				/*auth::CreateAccountWeb account;
				if (account.ParseFromString(data) == false) {
					std::cout << "Could not parse buffer " << data.length() <<  std::endl;
				}*/

				/*auth::AuthenticateWeb login;
				login.ParseFromString(data);*/

				// Buffer after being deserialized.
				
				
				// Client gets disconnected if no bytes are received from it
				if (bytesReceived <= 0) {
					client.connected = false;
					closesocket(client.cSock);
					// FD_CLR(client.cSock, &g_ServerInfo.socksReadyForReading);
					std::cout << "Client disconnected...\n";
					break;
				}

				// Send message to all clients except the one it came from
				else {
					for (int i = 0; i < g_ServerInfo.clients.size(); i++) {
				
						SOCKET sock = g_ServerInfo.clients[i].cSock;
						if (sock != client.cSock) {
							int sendResult = send(sock, buf, buflen, 0);
							if (sendResult == SOCKET_ERROR) {
								continue;
							}
							ZeroMemory(buf, buflen);
						}	
					}	
				}
			}
		}
	}
	return 0;
}
	
std::string SelectServer::Time() {
	auto time = std::chrono::system_clock::now();
	std::time_t con = std::chrono::system_clock::to_time_t(time);
	std::string conTime =  std::ctime(&con);
	return conTime;
}

void SelectServer::ShutDown() {
	std::cout << "\nShutting down...";
	freeaddrinfo(g_ServerInfo.info);
	closesocket(g_ServerInfo.listenSock);
	WSACleanup();
}

int SelectServer::Receive(ClientInfo& client, const int buflen, char* buf) {

	// Receive buffer
	
	int bytesReceived = recv(client.cSock, (char*)&(client.buffer.m_Buffer[0]), buflen, 0);

	if (bytesReceived <= 0) {
		std::cout << "Recv nothing." << std::endl;
	}
	else if(bytesReceived > 0) {
		if (client.buffer.m_Buffer.size() >= 4) {
			auth::CreateAccountWeb desUser;
			unsigned int temp = 0;
			unsigned int packageLength = client.buffer.ReadUInt32LE(temp);
			temp = 4;
			unsigned int messageId = client.buffer.ReadUInt32LE(temp);
			/*temp = 8;
			unsigned int messageLength = client.buffer.ReadUInt32LE(temp);
			std::string serializedUser(client.buffer.m_Buffer.begin() + 12,
				client.buffer.m_Buffer.begin() + messageLength + 12);*/

			temp = 8;
			unsigned int usrDataSize = client.buffer.ReadUInt32LE(temp);
			temp = 8;
			std::string serializedUser(client.buffer.m_Buffer.begin() + 16,
				client.buffer.m_Buffer.begin() + usrDataSize + 16);
			bool result = false;
			result = desUser.ParseFromString(serializedUser);
			if (result == false) {
				printf(" Failed to deserialize user.\n");
			}
			printf(" Testing %d \n", packageLength);
			printf(" Testing %d \n", messageId);
			std::cout << serializedUser;
		}
	}
	return bytesReceived;
}