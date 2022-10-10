#include "Buffer.h"

#include <iostream>

Buffer::Buffer(size_t size) : m_WriteIndex(0), m_ReadIndex(0) {
	m_Buffer.resize(size);
}

void Buffer::WriteInt32LE(std::size_t index, int32_t value) {
	m_Buffer[index] = value;
	m_Buffer[index + 1] = value >> 8;
	m_Buffer[index + 2] = value >> 16;
	m_Buffer[index + 3] = value >> 24;
}

uint32_t Buffer::ReadUInt32LE(std::size_t index) {
	uint32_t value = m_Buffer[index];
	value += m_Buffer[index + 1] << 8;
	value += m_Buffer[index + 2] << 16;
	value += m_Buffer[index + 3] << 24;
	return value;
}