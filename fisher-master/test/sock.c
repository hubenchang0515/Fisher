#include <sock.h>

uint8_t readU8(int fd)
{
	uint8_t data;
	ssize_t len = recv(fd, &data, 1, MSG_WAITALL);
	assert(len == 1);
	return data;
}

uint16_t readU16(int fd)
{
	uint16_t data;
	ssize_t len = recv(fd, &data, 2, MSG_WAITALL);
	assert(len == 2);
	return ntohs(data);
}

uint32_t readU32(int fd)
{
	uint32_t data;
	ssize_t len = recv(fd, &data, 4, MSG_WAITALL);
	assert(len == 4);
	return ntohl(data);
}

void writeU8(int fd, uint8_t data)
{
	ssize_t len = send(fd, &data, 1, 0);
	assert(len == 1);
}

void writeU16(int fd, uint16_t data)
{
	uint16_t netData = htons(data);
	ssize_t len = send(fd, &netData, 2, 0);
	assert(len == 2);
}

void writeU32(int fd, uint32_t data)
{
	uint32_t netData = htonl(data);
	ssize_t len = send(fd, &netData, 4, 0);
	assert(len == 4);
}


int8_t readI8(int fd)
{
    return (int8_t)readU8(fd);
}

int16_t readI16(int fd)
{
    return (int16_t)readU16(fd);
}

int32_t readI32(int fd)
{
    return (int32_t)readU32(fd);
}

void writeI8(int fd, int8_t data)
{
    writeU8(fd, (uint8_t)data);
}

void writeI16(int fd, int16_t data)
{
    writeU16(fd, (uint16_t)data);
}

void writeI32(int fd, int32_t data)
{
    writeU32(fd, (uint32_t)data);
}