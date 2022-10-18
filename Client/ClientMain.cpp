#include "Client.h"

int main(int argc, char** argv) {
	Client client;
	int result = client.Initialize();
	
	if (result != 0) {
		return result;
	}
	//client.FetchUserName();
	client.I_O();
	client.ShutDown();

	return 0;
	
}