#pragma once

#ifdef _WIN32
    #ifdef OVEN_BUILD_DLL
        #define OVEN_API __declspec(dllexport)
    #else
        #define OVEN_API __declspec(dllimport)
    #endif
#else
    #define OVEN_API __attribute__((visibility("default")))
#endif