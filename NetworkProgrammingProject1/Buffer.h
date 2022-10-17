#pragma once

#include <vector>

class Buffer {
public:
	Buffer();
	Buffer(size_t size);
	~Buffer();

	void WriteInt32LE(std::size_t index, int32_t value);
	void WriteInt32LE(int32_t value);
	uint32_t ReadUInt32LE(std::size_t index);
	uint32_t ReadUInt32LE();

private:
	std::vector<uint8_t> m_Buffer;

	int m_WriteIndex;
	int m_ReadIndex;
};
