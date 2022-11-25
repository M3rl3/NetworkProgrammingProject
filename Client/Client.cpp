#include "Client.h"

#include "../MessageType.h"
#include "auth.pb.h"

struct Header {
	uint32_t packetLength;
	uint32_t messageType;
	uint32_t usrDataLength;
};

struct Content {
	std::string usrData;
};

struct Packet {
	Header header;
	Content content;
};

std::vector<uint8_t> CreatePacket(int messageType, std::string serializedUsrData, std::string message) {
	Packet pkt;

	pkt.header.messageType = messageType;

	pkt.content.usrData = serializedUsrData;

	pkt.header.usrDataLength = pkt.content.usrData.size();

	pkt.header.packetLength =
		sizeof(Header) +
		sizeof(pkt.content.usrData.size()) + pkt.content.usrData.size();

	Buffer buffer(128);

	buffer.WriteUInt32LE(pkt.header.packetLength);
	buffer.WriteUInt32LE(pkt.header.messageType);
	buffer.WriteUInt32LE(pkt.content.usrData.size());
	buffer.m_Buffer.insert(buffer.m_Buffer.end(), pkt.content.usrData.begin(), pkt.content.usrData.end());
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

	DWORD NonBlock = 1;
	result = ioctlsocket(g_ClientInfo.sock, FIONBIO, &NonBlock);
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
	MessageType message_type;

yes:
	std::cout << "Create a new accout/Log in: " << std::endl;
	std::cout << "1. Create a new account" << std::endl;
	std::cout << "2. Log In" << std::endl;
	std::cout << "\n> ";

	int input_type;
	
	std::cin >> input_type;
	
	if (input_type == 1) {
		message_type = REGISTER;
	}
	else if (input_type == 2) {
		message_type = LOGIN;
	}
	else {
		std::cout << "\nInvalid Input" << std::endl;
		goto yes;
	}

	std::string email;
	std::string password;

	std::cout << "\nEmail: ";
	std::cin >> email;

	std::cout << "\nPassword: ";
	std::cin >> password;

	switch (message_type)
	{
	case REGISTER:
		if (Register(email, password)) {
			std::cout << "Registeration successful" << std::endl;
			std::cout << "Welcome " << email << std::endl;
		}
		else {
			std::cout << "Registeration Failed." << std::endl;
		}
		break;
	case LOGIN:
		if (Login(email, password)) {
			std::cout << "Login successful" << std::endl;
			std::cout << "Welcome " << email << std::endl;
		}
		else {
			std::cout << "Login Failed." << std::endl;
		}
		break;
	default:
		std::cout << "Whoops, something went wrong.";
		return 1;
		break;
	}

	message_type = SEND_MESSAGE;

	const int buflen = 256;
	char buf[buflen];
	
	bool dataSent = true;

	std::cout << "Sending message to server...\n";

	do {
		std::string input;

		std::cout << "> ";
		while (true) {
			if (kbhit()) {

				int key = getch();

				if (key == 13) {
					std::ostringstream ss;
					ss << email << " : " << input;
					std::string message = ss.str();
					
					Buffer buffer;
					message = buffer.WriteString(message, 0);

					int sendResult = send(g_ClientInfo.sock, message.c_str(), message.size() + 1, 0);
					if (sendResult == SOCKET_ERROR) {
						break;
					}
					input = "";
				}
				else if (key == 8) {
					input.pop_back();
				}
				else if (key == 27) {
					dataSent = false;
				}
				else {
					input += (char)key;
					std::cout << input << std::endl;
				}
			}
			
			ZeroMemory(buf, buflen);
			int bytesReceived = recv(g_ClientInfo.sock, buf, buflen, 0);
			
			if (bytesReceived > 0)
			{
				std::string recvBuf = buf;
				Buffer myBuf(recvBuf.length());
				recvBuf = myBuf.ReadString(recvBuf);
				std::cout << "> " << recvBuf << std::endl;
			}
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

bool Client::Login(std::string email, std::string password) {

	auth::AuthenticateWeb login;
	login.set_requestid(0);
	login.set_email(email);
	login.set_plaintextpassword(password);

	std::string serializedLoginInfo;
	login.SerializeToString(&serializedLoginInfo);
	
	Packet pkt;

	pkt.header.messageType = 0;

	pkt.content.usrData = serializedLoginInfo;

	pkt.header.usrDataLength = pkt.content.usrData.size();

	pkt.header.packetLength =
		sizeof(Header) +
		sizeof(pkt.content.usrData.size()) + pkt.content.usrData.size();

	Buffer buffer = Buffer(12);

	buffer.WriteUInt32LE(pkt.header.packetLength);
	buffer.WriteUInt32LE(pkt.header.messageType);
	buffer.WriteUInt32LE(pkt.content.usrData.size());
	buffer.m_Buffer.insert(buffer.m_Buffer.end(), pkt.content.usrData.begin(), pkt.content.usrData.end());

	int sendResult;
	sendResult = send(g_ClientInfo.sock, (const char*)buffer.m_Buffer.data(), buffer.m_Buffer.size(), 0);
	if (sendResult == SOCKET_ERROR) {
		std::cout << "Could not send login info over socket." << std::endl;
		return false;
	}
	return true;
}

bool Client::Register(std::string email, std::string password) {
	
	auth::CreateAccountWeb account;
	account.set_requestid(0);
	account.set_email(email);
	account.set_plaintextpassword(password);

	std::string serializedAccountInfo;
	account.SerializeToString(&serializedAccountInfo);

	Packet pkt;

	pkt.header.messageType = 1;

	pkt.content.usrData = serializedAccountInfo;

	pkt.header.usrDataLength = pkt.content.usrData.size();

	pkt.header.packetLength =
		sizeof(Header) +
		sizeof(pkt.content.usrData.size()) + pkt.content.usrData.size();

	Buffer buffer = Buffer(12);

	buffer.WriteUInt32LE(pkt.header.packetLength);
	buffer.WriteUInt32LE(pkt.header.messageType);
	buffer.WriteUInt32LE(pkt.content.usrData.size());
	// buffer.WriteString(pkt.content.usrData);
	buffer.m_Buffer.insert(buffer.m_Buffer.end(), pkt.content.usrData.begin(), pkt.content.usrData.end());

	int sendResult;
	sendResult = send(g_ClientInfo.sock,(const char *)buffer.m_Buffer.data(), buffer.m_Buffer.size(), 0);
	if (sendResult == SOCKET_ERROR) {
		std::cout << "Could not send registeration info over socket." << std::endl;
		return false;
	}
	return true;
}