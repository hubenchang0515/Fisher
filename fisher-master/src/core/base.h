#ifndef FISHER_LOG_H
#define FISHER_LOG_H

#include <SDL2/SDL.h>
#include <thread>

// GCC: https://sourceforge.net/p/predef/wiki/OperatingSystems/
#if defined(__ANDROID__) || defined(__ANDROID_API__)
    #define OS_ANDROID
#elif defined(__linux__) || defined(linux) || defined(__linux)
    #define OS_LINUX
#elif defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__))
    #define OS_MAC
#elif defined(BSD) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    #define OS_BSD
#elif defined(_WIN16) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
    #define OS_WIN
#endif

// LOG
#define LOG(fmt, ...) SDL_Log(" [%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define WAR(fmt, ...) SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, " [%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define ERR(fmt, ...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, " [%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#endif