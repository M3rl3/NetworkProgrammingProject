#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <chrono>
#include <string>

#include "Buffer.h"

#pragma comment(lib, "Ws2_32.lib")

#define LOCAL_HOST "127.0.0.1"
#define DEFAULT_PORT "54673"

class Client {
	struct ClientInfo {
		struct addrinfo* info = NULL;
		struct addrinfo* pointer = NULL;
		struct addrinfo hints;
		SOCKET sock = INVALID_SOCKET;
	}g_ClientInfo;

	WSADATA wsa;
	int result;

public:
	int Initialize();
	int I_O();
	void ShutDown();
	
};