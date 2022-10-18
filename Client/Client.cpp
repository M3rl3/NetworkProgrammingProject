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

	/*DWORD NonBlock = 1;
	result = ioctlsocket(g_ClientInfo.sock, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) {
		printf("ioctlsocket to failed with error: %d\n", WSAGetLastError());
		closesocket(g_ClientInfo.sock);
		freeaddrinfo(g_ClientInfo.info);
		WSACleanup();
		return 1;
	}*/

	const int recvBufLen = 256;
	char recvBuf[recvBufLen];
	

	const int buflen = 256;
	char buf[buflen];

	std::string uName;
	std::string input;
	char ch = '1';

	std::ostringstream ss;
	
	std::cout << "\nEnter username: ";

	getline(std::cin, uName);
	Buffer myBuf(uName.length());
	myBuf.WriteString(uName);

	std::cout << "Sending message to server...\n";
	do {
		std::cout << "> ";
		getline(std::cin, input);
		Buffer myBuf(input.length());
		myBuf.WriteString(input);
		/*if (_kbhit()) {
			ch = _getch();
			input += ch;
		}*/
		ss << uName << " : " << input << " | " << Time();
		std::string stream = ss.str();
		//Exits the loop if input buffer size is 0
		if (input.size() > 0) {
	
			int sendResult = send(g_ClientInfo.sock, input.c_str(), input.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				//Reset the buffer
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
	} while (ch != '0');

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