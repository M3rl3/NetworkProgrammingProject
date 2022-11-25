#include "AuthServer.h"

AuthServer::AuthServer() {

}

AuthServer::~AuthServer() {

}

int AuthServer::Initialize() {

	if (SQLConnect() == false) {
		std::cout << "Unable to connect to SQL server." << std::endl;
		return 1;
	}
	DisplayData();

	// Initialization 
	WSADATA wsaData;
	int result;

	printf("\nWSAStartup . . . ");
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

	if (UpdateTable("rameez@mail.com", "whatever", "securepassword", 3)) {
		std::cout << "Successfully updated table contents" << std::endl;
	}

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
		const int buflen = 256;
		char buf[buflen];

		if (FD_ISSET(g_AuthInfo.chatServer.socket, &g_AuthInfo.socksReadyForReading)) {
			int recvResult = recv(g_AuthInfo.chatServer.socket, buf, buflen, 0);
			if (recvResult == 0) {
				printf("Chat Server disconnected.\n");
				g_AuthInfo.chatServer.connected = false;
				FD_ZERO(g_AuthInfo.chatServer.socket);
				continue;
			}
		}	
	}
}

void AuthServer::ShutDown() {
	// Close
	printf("Closing . . . \n");
	closesocket(g_AuthInfo.listenSock);
	closesocket(g_AuthInfo.chatServer.socket);
	WSACleanup();
	SQLDisconnect();
	printf("Success!\n");
}

bool AuthServer::SQLConnect() {
	printf("Retrieving ccp-conn-sql driver . . . ");
	try {
		sqlDriver = sql::mysql::get_driver_instance();
	}
	catch (sql::SQLException e) {
		printf("Failed to get_driver_instance: %s\n", e.what());
		return false;
	}
	printf("Success!\n");

	printf("Connecting to database . . . ");

	// SQL server settings
	try {
		sql::SQLString hostName("127.0.0.1:3306");
		sql::SQLString userName("root");
		sql::SQLString password("root");
		con = sqlDriver->connect(hostName, userName, password);
		con->setSchema("schema");
	}
	catch (sql::SQLException e) {
		printf("Failed to connect to database: %s\n", e.what());
		return false;
	}
	printf("Success!\n");

	printf("Executing query statement . . . ");

	try {
		statement = con->createStatement();
		insertStatement = con->prepareStatement(
			"INSERT INTO web_auth (email, salt, hashed_password, userID) VALUES (?, ?, ?, ?);");
	}
	catch (sql::SQLException e) {
		printf("Failed to create statements: %s\n", e.what());
		return false;
	}
	printf("Success!\n");

	return true;
}

bool AuthServer::UpdateTable(const char* email, const char* salt, const char* hashed_password, int userID) {
	insertStatement->setString(1, email);
	insertStatement->setString(2, salt);
	insertStatement->setString(3, hashed_password);
	insertStatement->setInt(4, userID);

	try {
		insertStatement->execute();
	}
	catch (sql::SQLException e) {
		printf("Failed to add user to the database table 'web_auth': %s\n", e.what());
		return false;
	}


	// Automatically updates creation date and last login
	insertStatement = con->prepareStatement(
		"INSERT INTO user (creation_date, last_login, userID) VALUES (now(), now(), ?);");

	
	insertStatement->setInt(1, userID);

	try {
		insertStatement->execute();
	}
	catch (sql::SQLException e) {
		printf("Failed to add user to the database table 'user': %s\n", e.what());
		return false;
	}
	printf("Successfully added a user to database tables!\n");
	return true;
}

void AuthServer::DisplayData() {
	printf("Reading table data . . . ");
	try {
		resultSet = statement->executeQuery("SELECT * FROM web_auth;");
	}
	catch (sql::SQLException e) {
		printf("Failed to query our database: %s\n", e.what());
		return;
	}
	printf("Success!\n");
	printf("Retrieved % d rows from the database", (int)resultSet->rowsCount());
	printf("\n");

	while (resultSet->next()) {
		int id = resultSet->getInt("id");
		sql::SQLString email = resultSet->getString("email");
		sql::SQLString salt = resultSet->getString("salt");
		sql::SQLString hashed_password = resultSet->getString("hashed_password");
		int userID = resultSet->getInt("userID");

		std::cout << id << " " << email << " " << salt << " " << userID << std::endl;
	}
}

void AuthServer::SQLDisconnect() {
	try {
		con->close();
	}
	catch (sql::SQLException e) {
		printf("Failed to close the connection to our database: %s\n", e.what());
		return;
	}
	printf("Successfully closed the connection to our Database!\n");

	delete statement;
	delete resultSet;
	delete insertStatement;
}