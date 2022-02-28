#include <stdlib.h>
#include "image.h"

image_t* image_alloc(int32_t width, int32_t height)
{
    image_t* image = malloc(sizeof(image_t));
    image->data = malloc(3 * width * height);
    image->width = width;
    image->height = height;
    return image;
}

void image_free(image_t* image)
{
    free(image->data);
    free(image);
}