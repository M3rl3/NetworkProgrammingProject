#include "Client.h"

int Client::Initialize() {
	
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
	/*int bufflen = 10;
	Buffer buf(bufflen);

	buf.WriteInt32LE(0, 100);*/
	
	const int buflen = 256;
	char buf[buflen];

	std::string input;
	
	std::cout << "Sending message to server...\n";
	do {
		std::cout << "> ";
		getline(std::cin, input);

		if (input.size() > 0) {
			int sendResult = send(g_ClientInfo.sock, input.c_str(), input.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				ZeroMemory(buf, buflen);
				int bytesReceived = recv(g_ClientInfo.sock, buf, buflen, 0);
				if (bytesReceived > 0)
				{
					std::cout << "SERVER> " << std::string(buf, 0, bytesReceived) << std::endl;
				}
			}
			else {
				std::cout << "\nExit with code " << WSAGetLastError() << "." << std::endl;
			}
		}
	} while (input.size() > 0);

	return 0;
}

void Client::ShutDown() {
	std::cout << "\nShutting Down...";
	freeaddrinfo(g_ClientInfo.info);
	closesocket(g_ClientInfo.sock);
	WSACleanup();
}