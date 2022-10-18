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

#include "Buffer.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable : 4996)

#define LOCAL_HOST "127.0.0.1"
#define DEFAULT_PORT "54673"

class SelectServer {
	
	struct ClientInfo {
		SOCKET cSock;
		std::string name;
		bool connected;
	};

	struct ServerInfo {
		struct addrinfo* info = NULL;
		struct addrinfo hints;
		SOCKET listenSock = INVALID_SOCKET;
		SOCKET clientSock = INVALID_SOCKET;
		fd_set activeSock;
		fd_set socksReadyForReading;
		unsigned int clientsConnected = 0;
		std::string u_Name;
		std::vector<ClientInfo> clients;
	}g_ServerInfo;

public:

	SelectServer();
	~SelectServer();

	std::string Time();
	int Initialize();
	int I_O();
	std::string HostName();
	void ShutDown();
};
