#include "SelectServer.h"

SelectServer::SelectServer() {
	
}

SelectServer::~SelectServer() {

}

int SelectServer::Initialize() {

	std::cout << "Startup running...\n";
	result = WSAStartup(MAKEWORD(2, 2), &wsa);

	FD_ZERO(&g_ServerInfo.activeSock);
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
		std::cout << "\nExit with code " << result << "." << std::endl;
		WSACleanup();
		return 1;
	}
	
	std::cout << "Creating socket...\n";
	g_ServerInfo.listenSock = socket(g_ServerInfo.info->ai_family, g_ServerInfo.info->ai_socktype, g_ServerInfo.info->ai_protocol);
	if (g_ServerInfo.listenSock == INVALID_SOCKET) {
		std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ServerInfo.info);
		WSACleanup();
		return 1;
	}

	std::cout << "Binding socket...\n";
	result = bind(g_ServerInfo.listenSock, g_ServerInfo.info->ai_addr, g_ServerInfo.info->ai_addrlen);
	if (result == SOCKET_ERROR) {
		std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ServerInfo.info);
		closesocket(g_ServerInfo.listenSock);
		WSACleanup();
		return 1;
	}

	std::cout << "Listening on socket...\n";
	result = listen(g_ServerInfo.listenSock, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
		freeaddrinfo(g_ServerInfo.info);
		closesocket(g_ServerInfo.listenSock);
		WSACleanup();
		return 1;
	}
	
	return 0;
}

int SelectServer::I_O() {
	
	int selResult;
	struct timeval t_val;
	t_val.tv_sec = 0;
	t_val.tv_usec = 500 * 1000;

	while (1) {

		FD_ZERO(&g_ServerInfo.socksReadyForReading);
		FD_SET(g_ServerInfo.listenSock, &g_ServerInfo.socksReadyForReading);

		for (int i = 0; i < g_ServerInfo.clients.size(); i++) {
			ClientInfo& c = g_ServerInfo.clients[i];
			if (c.connected = true) FD_SET(c.cSock, &g_ServerInfo.socksReadyForReading);
		}

		//std::cout << "Calling select...\n";
		selResult = select(0, &g_ServerInfo.socksReadyForReading, NULL, NULL, &t_val);
		if (selResult == SOCKET_ERROR) {
			std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
			freeaddrinfo(g_ServerInfo.info);
			closesocket(g_ServerInfo.listenSock);
			WSACleanup();
			return 1;
		}

		if (FD_ISSET(g_ServerInfo.listenSock, &g_ServerInfo.socksReadyForReading)) {
			std::cout << "Accepting connections...\n";
			g_ServerInfo.clientSock = accept(g_ServerInfo.listenSock, NULL, NULL);
			if (g_ServerInfo.clientSock == INVALID_SOCKET) {
				std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
			}
			else {
				ClientInfo c;
				c.cSock = g_ServerInfo.clientSock;
				c.connected = true;
				g_ServerInfo.clients.push_back(c);
			}
		}

		for (int i = 0; i < g_ServerInfo.clients.size(); i++) {
			ClientInfo& c = g_ServerInfo.clients[i];
			if (c.connected == false) continue;
			if ((FD_ISSET(c.cSock, &g_ServerInfo.socksReadyForReading))) {
				const int buflen = 256;
				char buf[buflen];

				std::cout << "Receiveing buffer from client...\n";
				int recvResult = recv(c.cSock, buf, buflen, 0);
				if (recvResult == SOCKET_ERROR) {
					std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
				}

				if (recvResult == 0) {
					c.connected = false;
					std::cout << "Client Disconnected.\n";
					continue;
				}

				std::cout << buf << " recv time: " << Time();

				int sendResult = send(c.cSock, buf, buflen, 0);
				if (sendResult == SOCKET_ERROR) {
					std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
				}
			}
		}
	}
}

//void SelectServer::HostName() {
//	char host[NI_MAXHOST];		// Client's remote name
//	char service[NI_MAXSERV];	// Service (i.e. port) the client is connect on
//
//	ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
//	ZeroMemory(service, NI_MAXSERV);
//
//	if (getnameinfo(&g_ServerInfo.clientSock, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
//	{
//		std::cout << host << " connected on port " << service << std::endl;
//	}
//	else
//	{
//		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
//		std::cout << host << " connected on port " <<
//			ntohs(client.sin_port) << std::endl;
//	}
//}

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
	closesocket(g_ServerInfo.clientSock);
	WSACleanup();
}