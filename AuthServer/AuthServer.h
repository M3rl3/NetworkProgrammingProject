#pragma once

#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>

#include "../Buffer/Buffer.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable : 4996)

#define LOCAL_HOST "127.0.0.1"
#define DEFAULT_PORT "3306"

class AuthServer {
public:
	AuthServer();
	~AuthServer();

	struct ChatServerInformation {
		SOCKET socket;
		bool connected = false;
	};

	struct AuthInfo {
		struct addrinfo* info = NULL;
		struct addrinfo hints;
		SOCKET listenSock = INVALID_SOCKET;
		SOCKET chatsServerSock = INVALID_SOCKET;
		fd_set activeSock;
		fd_set socksReadyForReading;
		std::string email;
		std::string password;

		ChatServerInformation chatServer;
	}g_AuthInfo;

	int Initialize();
	int I_O();
	void ShutDown();
};