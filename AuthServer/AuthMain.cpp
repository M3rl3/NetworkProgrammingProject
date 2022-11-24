#include "AuthServer.h"

int main(int argc, char** argv)
{
	AuthServer authServer;

	authServer.Initialize();
	authServer.I_O();
	authServer.ShutDown();

	return 0;
}