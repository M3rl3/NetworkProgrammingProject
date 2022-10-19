#include "Buffer.h"

#include <iostream>

Buffer::Buffer() {

}

Buffer::Buffer(size_t size) : m_WriteIndex(0), m_ReadIndex(0) {
	m_Buffer.resize(size);
	m_StrBuffer.resize(size);
}

Buffer::~Buffer() {

}

//Unsigned Int
void Buffer::WriteUInt32LE(std::size_t index, uint32_t value) {
	m_Buffer[index] = value;
	m_Buffer[index + 1] = value >> 8;
	m_Buffer[index + 2] = value >> 16;
	m_Buffer[index + 3] = value >> 24;
}

void Buffer::WriteUInt32LE(uint32_t value)
{
	m_Buffer[m_WriteIndex++] = value;
	m_Buffer[m_WriteIndex++] = value >> 8;
	m_Buffer[m_WriteIndex++] = value >> 16;
	m_Buffer[m_WriteIndex++] = value >> 24;
}

uint8_t Buffer::WriteUInt32LE(uint32_t value, int temp)
{
	m_Buffer[m_WriteIndex++] = value;
	m_Buffer[m_WriteIndex++] = value >> 8;
	m_Buffer[m_WriteIndex++] = value >> 16;
	m_Buffer[m_WriteIndex++] = value >> 24;
	
	return m_Buffer[m_WriteIndex];
}

uint32_t Buffer::ReadUInt32LE(std::size_t index) {
	uint32_t value = m_Buffer[index];
	value += m_Buffer[index + 1] << 8;
	value += m_Buffer[index + 2] << 16;
	value += m_Buffer[index + 3] << 24;
	return value;
}

uint32_t Buffer::ReadUInt32LE()
{
	uint32_t value = m_Buffer[m_ReadIndex++];
	value |= m_Buffer[m_ReadIndex++] << 8;
	value |= m_Buffer[m_ReadIndex++] << 16;
	value |= m_Buffer[m_ReadIndex++] << 24;
	return value;
}

uint32_t Buffer::ReadUInt32LE(uint32_t value)
{
	value = m_Buffer[m_ReadIndex++];
	value |= m_Buffer[m_ReadIndex++] << 8;
	value |= m_Buffer[m_ReadIndex++] << 16;
	value |= m_Buffer[m_ReadIndex++] << 24;
	return value;
}

//Signed Int
void Buffer::WriteInt32LE(std::size_t index, int32_t value) {
	m_Buffer[index] = value;
	m_Buffer[index + 1] = value >> 8;
	m_Buffer[index + 2] = value >> 16;
	m_Buffer[index + 3] = value >> 24;
}

void Buffer::WriteInt32LE(int32_t value) {
	m_Buffer[m_WriteIndex++] = value;
	m_Buffer[m_WriteIndex++] = value >> 8;
	m_Buffer[m_WriteIndex++] = value >> 16;
	m_Buffer[m_WriteIndex++] = value >> 24;
}

int32_t Buffer::ReadInt32LE(std::size_t index) {
	int32_t value = m_Buffer[index];
	value += m_Buffer[index + 1] << 8;
	value += m_Buffer[index + 2] << 16;
	value += m_Buffer[index + 3] << 24;
	return value;
}

int32_t Buffer::ReadInt32LE() {
	int32_t value = m_Buffer[m_ReadIndex++];
	value |= m_Buffer[m_ReadIndex++] << 8;
	value |= m_Buffer[m_ReadIndex++] << 16;
	value |= m_Buffer[m_ReadIndex++] << 24;
	return value;
}

//Unsigned short
void Buffer::WriteUInt16LE(std::size_t index, uint16_t value) {
	m_Buffer[index] = value;
	m_Buffer[index + 1] = value >> 4;
	m_Buffer[index + 2] = value >> 8;
	m_Buffer[index + 3] = value >> 12;
}

void Buffer::WriteUInt16LE(uint16_t value)
{
	m_Buffer[m_WriteIndex++] = value;
	m_Buffer[m_WriteIndex++] = value >> 4;
	m_Buffer[m_WriteIndex++] = value >> 8;
	m_Buffer[m_WriteIndex++] = value >> 12;
}

uint16_t Buffer::ReadUInt16LE(std::size_t index) {
	uint16_t value = m_Buffer[index];
	value += m_Buffer[index + 1] << 4;
	value += m_Buffer[index + 2] << 8;
	value += m_Buffer[index + 3] << 12;
	return value;
}

uint16_t Buffer::ReadUInt16LE()
{
	uint16_t value = m_Buffer[m_ReadIndex++];
	value |= m_Buffer[m_ReadIndex++] << 4;
	value |= m_Buffer[m_ReadIndex++] << 8;
	value |= m_Buffer[m_ReadIndex++] << 12;
	return value;
}

//Signed short
void Buffer::WriteInt16LE(std::size_t index, int16_t value) {
	m_Buffer[index] = value;
	m_Buffer[index + 1] = value >> 4;
	m_Buffer[index + 2] = value >> 8;
	m_Buffer[index + 3] = value >> 12;
}

void Buffer::WriteInt16LE(int16_t value) {
	m_Buffer[m_WriteIndex++] = value;
	m_Buffer[m_WriteIndex++] = value >> 4;
	m_Buffer[m_WriteIndex++] = value >> 8;
	m_Buffer[m_WriteIndex++] = value >> 12;
}

int16_t Buffer::ReadInt16LE(std::size_t index) {
	int16_t value = m_Buffer[index];
	value += m_Buffer[index + 1] << 4;
	value += m_Buffer[index + 2] << 8;
	value += m_Buffer[index + 3] << 12;
	return value;
}

int16_t Buffer::ReadInt16LE() {
	int16_t value = m_Buffer[m_ReadIndex++];
	value |= m_Buffer[m_ReadIndex++] << 4;
	value |= m_Buffer[m_ReadIndex++] << 8;
	value |= m_Buffer[m_ReadIndex++] << 12;
	return value;
}

//String
void Buffer::WriteString(std::size_t index, std::string value) {
	for (int i = 0; i < index; i++) {
		m_Buffer[i] = ((unsigned char)value[i]) << 1;
	}
	//m_StrBuffer = value;
}

void Buffer::WriteString(std::string value) {
	for (int i = 0; i < value.length(); i++) {
		m_Buffer[i] = ((unsigned char)value[i]) << 1;
	}
	m_StrBuffer = value;
}

std::string Buffer::WriteString(std::string value, int temp) {
	for (int i = 0; i < value.length(); i++) {
		value[i] = ((unsigned char)value[i]) << 1;
	}
	m_StrBuffer = value;
	return value;
}

std::string Buffer::ReadString(std::size_t index) {
	std::string value;
	for (int i = 0; i < index; i++) {
		value[i] = ((unsigned char)m_StrBuffer[i]) >> 1;
	}
	return value;
}

std::string Buffer::ReadString(std::string value) {
	for (int i = 0; i < value.length(); i++) {
		value[i] = ((unsigned char)value[i]) >> 1;
	}
	return value;
}

std::string Buffer::ReadString() {
	std::string value = m_StrBuffer;
	for (int i = 0; i < value.length(); i++) {
		value[i] = ((unsigned char)value[i]) >> 1;
	}
	return value;
}
