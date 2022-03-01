#ifndef SOCK_H
#define SOCK_H

#include <assert.h>
#include <unistd.h>  
#include <arpa/inet.h>
#include <sys/socket.h>

uint8_t readU8(int fd);
uint16_t readU16(int fd);
uint32_t readU32(int fd);

int8_t readI8(int fd);
int16_t readI16(int fd);
int32_t readI32(int fd);

void writeU8(int fd, uint8_t data);
void writeU16(int fd, uint16_t data);
void writeU32(int fd, uint32_t data);

void writeI8(int fd, int8_t data);
void writeI16(int fd, int16_t data);
void writeI32(int fd, int32_t data);

#endif