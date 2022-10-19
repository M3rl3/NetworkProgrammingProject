#include "Buffer.h"

struct Header {
	int packetLength;
	int messageLength;
	int roomNameLength;
};

struct Content {
	std::string roomName;
	std::string message;
};

struct Packet {
	Header header;
	Content content;
};

Packet CreatePacket() {
	Packet pkt;
	pkt.content.roomName = "room";
	pkt.content.message = "You retarded.";

	pkt.header.roomNameLength = sizeof(pkt.content.roomName);
	pkt.header.messageLength = sizeof(pkt.content.message);

	pkt.header.packetLength = sizeof(pkt.header.roomNameLength) + sizeof(pkt.header.messageLength) +
		pkt.header.roomNameLength + pkt.header.messageLength;
	return pkt;
}

Buffer Serialize() {

	Packet pack = CreatePacket();
	Buffer buffer(pack.content.message.length());

	/*buffer.WriteUInt32LE(pack.header.packetLength);
	buffer.WriteUInt32LE(pack.header.messageLength);
	buffer.WriteUInt32LE(pack.header.roomNameLength);*/
	buffer.WriteString(pack.content.message);
	//buffer.WriteString(pack.content.roomName);
	return buffer;
}

void main(int argc, char** argv) {

	Buffer buf(50000);
	
	buf.WriteInt16LE(-123);		//signed short serialization
	buf.WriteInt32LE(-1234);	//signed int serialization
	
	std::cout << buf.ReadInt16LE();
	std::cout << "\n" << buf.ReadInt32LE();

	std::string myString = "Hello!";

	Buffer buf1(myString.length());
	buf1.WriteString(myString);	//string serialization
	
	std::cout << "\n" << buf1.ReadString();
}