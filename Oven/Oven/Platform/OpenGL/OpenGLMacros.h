#pragma once


#ifdef OVEN_DEBUG

    #include <type_traits>
    #include <glad/glad.h>
    #include <utility> 
    #include <Oven/Log.h>
    template<typename F>
    auto glCallImpl(F&& func, const char* expr) {
    while(glGetError() != GL_NO_ERROR);
    
    if constexpr (std::is_void_v<std::invoke_result_t<F>>) {
        func();
        while(GLenum error = glGetError()) {
            OVEN_CORE_ERROR("OpenGL Error {} in {}", error, expr);
        }
    } else {
        auto __ret = func();
        while(GLenum error = glGetError()) {
            OVEN_CORE_ERROR("OpenGL Error {} in {}", error, expr);
        }
        return __ret;
    }
}

#define GL_CALL(x) glCallImpl([&](){ return x; }, #x)
#else

    #define GL_CALL(x) x

#endif