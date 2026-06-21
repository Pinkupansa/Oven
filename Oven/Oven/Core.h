#pragma once
#include <memory>

#if OVEN_DYNAMIC_LINK
    #ifdef OVEN_PLATFORM_WINDOWS
        #ifdef OVEN_BUILD_DLL
            #define OVEN_API __declspec(dllexport)
        #else
            #define OVEN_API __declspec(dllimport)
        #endif
    #else
        #define OVEN_API __attribute__((visibility("default")))
#endif
#else 
#define OVEN_API
#endif
#ifdef OVEN_DEBUG
#define OVEN_ENABLE_ASSERT
#endif

#define BIT(x) (1 << x)

#define OVEN_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Oven{
    template<typename T> 
    using Scope = std::unique_ptr<T>;

    template<typename T> 
    using Ref = std::shared_ptr<T>;

}