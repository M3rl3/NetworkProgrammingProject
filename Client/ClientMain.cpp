#include "Client.h"

int main(int argc, char** argv) {
	Client client;
	int result = client.Initialize();
	if (result != 0) {
		return result;
	}
	client.I_O();
	result = client.Initialize();
	if (result != 0) {
		return result;
	}
	client.ShutDown();

	_getch();
	return 0;
	
}