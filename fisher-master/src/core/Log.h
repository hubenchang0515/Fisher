#ifndef FISHER_LOG_H
#define FISHER_LOG_H

#include <SDL2/SDL.h>

#define LOG(fmt, ...) SDL_Log(" [%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define ERR(fmt, ...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, " [%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#endif