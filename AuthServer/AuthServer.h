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

#include "jdbc/mysql_driver.h"
#include "jdbc/mysql_connection.h"
#include "jdbc/mysql_error.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/prepared_statement.h"
#include "jdbc/cppconn/resultset.h"

#include "../Buffer/Buffer.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable : 4996)

#define LOCAL_HOST "127.0.0.1"
#define DEFAULT_PORT "8888"

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

	bool SQLConnect();
	bool UpdateTable(const char* email, const char* salt, const char* hashed_password, int userID);
	void DisplayData();
	void SQLDisconnect();

private:
	sql::Driver* sqlDriver;
	sql::Connection* con;
	sql::ResultSet* resultSet;
	sql::Statement* statement;
	sql::PreparedStatement* insertStatement;
};