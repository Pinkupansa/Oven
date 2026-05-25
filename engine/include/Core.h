#pragma once

#ifdef OVEN_PLATFORM_WINDOWS
    #ifdef OVEN_BUILD_DLL
        #define OVEN_API __declspec(dllexport)
    #else
        #define OVEN_API __declspec(dllimport)
    #endif
#else
    #define OVEN_API __attribute__((visibility("default")))
#endif

#define BIT(x) (1 << x)