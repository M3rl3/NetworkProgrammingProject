#include "AuthServer.h"

AuthServer::AuthServer() {

}

AuthServer::~AuthServer() {

}

int AuthServer::Initialize() {
	// Initialization 
	WSADATA wsaData;
	int result;

	printf("WSAStartup . . . ");
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed with error %d\n", result);
		return 1;
	}
	else {
		printf("Success!\n");
	}

	ZeroMemory(&g_AuthInfo.hints, sizeof(g_AuthInfo.hints));
	g_AuthInfo.hints.ai_family = AF_INET;		// IPV4
	g_AuthInfo.hints.ai_socktype = SOCK_STREAM;	// Stream
	g_AuthInfo.hints.ai_protocol = IPPROTO_TCP;	// TCP
	g_AuthInfo.hints.ai_flags = AI_PASSIVE;

	printf("Creating our AddrInfo . . . ");
	result = getaddrinfo(NULL, DEFAULT_PORT, &g_AuthInfo.hints, &g_AuthInfo.info);
	if (result != 0) {
		printf("getaddrinfo failed with error: %d\n", result);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	// Create socket
	printf("Creating our Listen Socket . . . ");
	g_AuthInfo.listenSock = socket(g_AuthInfo.info->ai_family, g_AuthInfo.info->ai_socktype, g_AuthInfo.info->ai_protocol);
	if (g_AuthInfo.listenSock == INVALID_SOCKET) {
		printf("Socket failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(g_AuthInfo.info);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	// Bind socket
	printf("Calling Bind . . . ");
	result = bind(g_AuthInfo.listenSock, g_AuthInfo.info->ai_addr, (int)g_AuthInfo.info->ai_addrlen);
	if (result == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(g_AuthInfo.info);
		closesocket(g_AuthInfo.listenSock);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}

	// Listen on socket
	printf("Calling Listen . . . ");
	result = listen(g_AuthInfo.listenSock, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(g_AuthInfo.info);
		closesocket(g_AuthInfo.listenSock);
		WSACleanup();
		return 1;
	}
	else {
		printf("Success!\n");
	}
}

int AuthServer::I_O() {

	struct timeval t_val;
	t_val.tv_sec = 0;
	t_val.tv_usec = 500 * 1000;

	bool executing = true;
	std::cout << "Calling select . . . \n";

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 500 * 1000; // 500 milliseconds, half a second

	int selectResult;

	while (executing) {
		
		FD_ZERO(&g_AuthInfo.socksReadyForReading);

		FD_SET(g_AuthInfo.listenSock, &g_AuthInfo.socksReadyForReading);

		if (g_AuthInfo.chatServer.connected) {
			FD_SET(g_AuthInfo.chatServer.socket, &g_AuthInfo.socksReadyForReading);
		}

		selectResult = select(0, &g_AuthInfo.socksReadyForReading, NULL, NULL, &tv);
		if (selectResult == SOCKET_ERROR) {
			printf("Select call failed with error: %d\n", WSAGetLastError());
			return 1;
		}
		printf(".");

		if (FD_ISSET(g_AuthInfo.listenSock, &g_AuthInfo.socksReadyForReading)) {

			// Accept call
			printf("\nCalling Accept . . . ");

			SOCKET chatServerSocket = accept(g_AuthInfo.listenSock, NULL, NULL);
			if (chatServerSocket == INVALID_SOCKET) {
				printf("Accept failed with error: %d\n", WSAGetLastError());
			}
			else {
				printf("Success!\n");
				g_AuthInfo.chatServer.socket = chatServerSocket;
				g_AuthInfo.chatServer.connected = true;
				printf("\nEstablished connection to chat server!\n");
			}
		}

		if (FD_ISSET(g_AuthInfo.chatServer.socket, &g_AuthInfo.socksReadyForReading)) {

			const int buflen = 128;
			char buf[buflen];

			int recvResult = recv(g_AuthInfo.chatServer.socket, buf, buflen, 0);
			if (recvResult == 0) {
				printf("Chat Server disconnected.\n");
				g_AuthInfo.chatServer.connected = false;
				continue;
			}
			printf("Message From the chat server:\n%s\n", buf);
		}	
	}
}


void AuthServer::ShutDown() {
	// Close
	printf("Closing . . . \n");
	freeaddrinfo(g_AuthInfo.info);
	closesocket(g_AuthInfo.listenSock);
	closesocket(g_AuthInfo.chatServer.socket);
	WSACleanup();
}
