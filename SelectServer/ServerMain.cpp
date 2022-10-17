#include "SelectServer.h"

int main(int argc, char** argv) {
	
	SelectServer server;
	int result = server.Initialize();
	if (result != 0) {
		return result;
	}
	result = server.I_O();
	if (result != 0) {
		return result;
	}
	server.ShutDown();
	
	return 0;
}