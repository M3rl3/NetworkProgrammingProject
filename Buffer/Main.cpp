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

	buf.WriteUInt16LE(456);
	buf.WriteInt32LE(1000);
	std::cout << buf.ReadUInt16LE();
	std::cout << buf.ReadInt32LE();
}