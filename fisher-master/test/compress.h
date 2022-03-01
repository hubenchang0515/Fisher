#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <jpeglib.h>

void compress(const void* rgb, int width, int height, void** jpeg, size_t* outSize);

#endif