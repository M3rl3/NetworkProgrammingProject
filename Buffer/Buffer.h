#pragma once

#include <iostream>
#include <vector>

class Buffer {

	std::vector<uint8_t> m_Buffer;
	std::string m_StrBuffer = "";

	int m_WriteIndex;
	int m_ReadIndex;

public:
	Buffer();
	Buffer(size_t size);
	~Buffer();

	inline void ResetBufferIndex() {
		m_Buffer.clear();
		m_ReadIndex = 0;
		m_WriteIndex = 0;
	}

	//Unsigned int 
	void WriteUInt32LE(std::size_t index, uint32_t value);
	void WriteUInt32LE(uint32_t value);

	uint32_t ReadUInt32LE(std::size_t index);
	uint32_t ReadUInt32LE();

	//Signed int
	void WriteInt32LE(std::size_t index, int32_t value);
	void WriteInt32LE(int32_t value);

	int32_t ReadInt32LE(std::size_t index);
	int32_t ReadInt32LE();

	//Unsigned short
	void WriteUInt16LE(std::size_t index, uint16_t value);
	void WriteUInt16LE(uint16_t value);

	uint16_t ReadUInt16LE(std::size_t index);
	uint16_t ReadUInt16LE();

	//Signed short
	void WriteInt16LE(std::size_t index, int16_t value);
	void WriteInt16LE(int16_t value);

	int16_t ReadInt16LE(std::size_t index);
	int16_t ReadInt16LE();

	//String
	void WriteString(std::size_t index, std::string value);
	void WriteString(std::string value);

	std::string ReadString(std::size_t index);
	std::string ReadString(std::string value);
	std::string ReadString();
};
