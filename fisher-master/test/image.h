#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

// only RGB, no padding
typedef struct image_t
{
    int32_t width;
    int32_t height;
    uint8_t* data;
}image_t;

image_t* image_alloc(int32_t width, int32_t height);
void image_free(image_t* image);

#endif // IMAGE_H