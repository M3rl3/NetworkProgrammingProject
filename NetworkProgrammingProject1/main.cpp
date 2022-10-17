#include "Buffer.h"

int main(int argc, char** argv) {
	Buffer buf(10);
	buf.WriteInt32LE(0, 500);
	int result = buf.ReadUInt32LE(0);
	return result;
}