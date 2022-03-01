#ifndef KEYSYM_H
#define KEYSYM_H

#include <stdint.h>

#define KEY_SYM_UNKNOWN UINT32_MAX
uint32_t get_x11_keysym(int32_t scancode);

#endif