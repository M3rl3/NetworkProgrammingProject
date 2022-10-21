#include "Buffer.h"

void main(int argc, char** argv) {

	Buffer buf(50000);
	
	buf.WriteInt16LE(-123);		//signed short serialization
	buf.WriteInt32LE(-1234);	//signed int serialization
	
	std::cout << buf.ReadInt16LE();				//signed short deserialization
	std::cout << "\n" << buf.ReadInt32LE();		//signed int deserialization
	
	Buffer buf2(4);
	std::string man = buf2.WriteString("Hey!", 0);  //string serialization
	std::cout << "\n" << man;						
	std::cout << "\n" << buf2.ReadString(man);		//string deserialization

	Buffer buf3(500); //test
	uint32_t myInt32 = buf3.WriteUInt32LE(69420, 0);
	std::cout << "\n" << myInt32;
	std::cout << "\n" << buf3.ReadUInt32LE(myInt32);
}