#ifndef __kernel_platform_h
#define __kernel_platform_h

#define UNKNOWN 0
#define IOS 1
#define DROID 2
#define LINUX 3
#define OSX 4
#define WEB 5
#define WINDOWS 6

#define OS DROID

#if defined(EMSCRIPTEN)
        #undef OS
        #define OS WEB
#elif defined(__APPLE__)

        #include "TargetConditionals.h"
        #if TARGET_IPHONE_SIMULATOR
                #undef OS
                #define OS IOS
        #elif TARGET_OS_IPHONE

                #undef OS
                #define OS IOS

        #elif TARGET_OS_MAC
                #undef OS
                #define OS OSX
        #endif

#elif defined(ANDROID)

        #undef OS
        #define OS DROID

#elif defined(WIN32) || defined(_WIN32)

        #undef OS
        #define OS WINDOWS

#elif defined(linux)

        #undef OS
        #define OS LINUX

#endif

#endif
