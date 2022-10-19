#include "SelectServer.h"

SelectServer::SelectServer() {

}

SelectServer::~SelectServer() {

}

int SelectServer::Initialize() {

	WSADATA wsa;
	int result;

	std::cout << "Startup running...\n";
	result = WSAStartup(MAKEWORD(2, 2), &wsa);

	FD_ZERO(&g_ServerInfo.socksReadyForReading);
	g_ServerInfo.clientsConnected = 0;

	if (result != 0) {
		std::cout << "\nExit with code " << result << "." << std::endl;
		return 1;
	}

	ZeroMemory(&g_ServerInfo.hints, sizeof(g_ServerInfo.hints));
	
	g_ServerInfo.hints.ai_family = AF_INET;
	g_ServerInfo.hints.ai_socktype = SOCK_STREAM;
	g_ServerInfo.hints.ai_protocol = IPPROTO_TCP;
	g_ServerInfo.hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(LOCAL_HOST, DEFAULT_PORT, &g_ServerInfo.hints, &g_ServerInfo.info);
	std::cout << "Getting address info...\n";
	if (result != 0) {
		std::cout << "\nFetching address info exit with code " << result << "." << std::endl;
		WSACleanup();
		return 1;
	}
	
	std::cout << "Creating socket...\n";
	//g_ServerInfo.listenSock = socket(g_ServerInfo.info->ai_family, g_ServerInfo.info->ai_socktype, g_ServerInfo.info->ai_protocol);
	g_ServerInfo.listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (g_ServerInfo.listenSock == INVALID_SOCKET) {
		std::cout << "\nSocket creation exit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ServerInfo.info);
		WSACleanup();
		return 1;
	}

	std::cout << "Binding socket...\n";
	result = bind(g_ServerInfo.listenSock, g_ServerInfo.info->ai_addr, g_ServerInfo.info->ai_addrlen);
	if (result == SOCKET_ERROR) {
		std::cout << "\nSocket bind exit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ServerInfo.info);
		closesocket(g_ServerInfo.listenSock);
		WSACleanup();
		return 1;
	}

	std::cout << "Listening on socket...\n";
	result = listen(g_ServerInfo.listenSock, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		std::cout << "\nListening on socket exit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ServerInfo.info);
		closesocket(g_ServerInfo.listenSock);
		WSACleanup();
		return 1;
	}
	std::cout << "Accepting connections...\n";
	
	return 0;
}

int SelectServer::I_O() {

	struct timeval t_val;
	int selResult;
	t_val.tv_sec = 0;
	t_val.tv_usec = 500 * 1000;

	bool executing = true;
	while (executing) {
		
		FD_ZERO(&g_ServerInfo.socksReadyForReading);
		FD_SET(g_ServerInfo.listenSock, &g_ServerInfo.socksReadyForReading);
		
		fd_set copy = g_ServerInfo.socksReadyForReading;


		for (int i = 0; i < g_ServerInfo.clients.size(); i++) {
			ClientInfo& client = g_ServerInfo.clients[i];
			if (client.connected) FD_SET(client.cSock, &g_ServerInfo.socksReadyForReading);
		}

		//std::cout << "Calling select...\n";
		selResult = select(0, &g_ServerInfo.socksReadyForReading, nullptr, nullptr, &t_val);
		if (selResult == SOCKET_ERROR) {
			std::cout << "\nSelect call exit with code " << WSAGetLastError() << "." << std::endl;
			freeaddrinfo(g_ServerInfo.info);
			closesocket(g_ServerInfo.listenSock);
			WSACleanup();
			return 1;
		}
		
		//Inbound connection
		if (FD_ISSET(g_ServerInfo.listenSock, &g_ServerInfo.socksReadyForReading)) {
	
			SOCKET clientSocket = accept(g_ServerInfo.listenSock, nullptr, nullptr);
			if (clientSocket == INVALID_SOCKET) {
				std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
			}
			else {
				ClientInfo client;
				client.connected = true;
				client.cSock = clientSocket;
				FD_SET(client.cSock, &g_ServerInfo.socksReadyForReading);
				
				std::string welMessage = "Welcome to the chat server!";
				send(client.cSock, welMessage.c_str(), welMessage.size() + 1, 0);

				g_ServerInfo.clients.push_back(client);
			}
		}

		std::string buffer;

		//Inbound message
		for (int i = 0; i < g_ServerInfo.clients.size(); i++) {
			
			ClientInfo client = g_ServerInfo.clients[i];
			
			if (client.connected == false) {
				continue;
			}

			if (FD_ISSET(client.cSock, &g_ServerInfo.socksReadyForReading)) {
				const int buflen = 256;
				char buf[buflen];
				ZeroMemory(buf, buflen);

				//Receive buffer
				int bytesReceived = recv(client.cSock, buf, buflen, 0);
				buffer = buf;

				//Deserialize the buffer
				Buffer myBuf(buffer.length());
				myBuf.ReadString(buffer);

				std::cout << "\n> " << buffer;
				
				if (bytesReceived <= 0) {
					client.connected = false;
					closesocket(client.cSock);
					FD_CLR(client.cSock, &g_ServerInfo.socksReadyForReading);
					std::cout << "Client disconnected...\n";
					break;
				}

				//Send message to all clients except the one it came from
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