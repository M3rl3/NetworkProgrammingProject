#include <iostream>

#include "Buffer.h"

int main(int argc, char** argv) {
	Buffer buffer(1000);
	buffer.WriteInt32LE(0, 500);
	std::cout << buffer.ReadUInt32LE(0);

	return 0;

	//125 = 01111101
	//5   = 0000010 1
	/*uint8_t yes = 5;
	std::cout << (yes >> 1) << std::endl;*/
}