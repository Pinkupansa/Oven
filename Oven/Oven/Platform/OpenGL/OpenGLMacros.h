#pragma once


#ifdef OVEN_DEBUG

    #include <type_traits>
    #include <glad/glad.h>
    #include <utility> 
    #include <Oven/Core/Log.h>
    static std::string getOpenGLErrorMessage(GLenum error) {
       switch (error) {
           case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION: texture shader is not bound!";
           case GL_INVALID_ENUM: return "GL_INVALID_ENUM: invalid enum value passed.";
           case GL_INVALID_VALUE: return "GL_INVALID_VALUE: invalid parameter value.";
           // Add more cases for other errors
           default: return "Unknown OpenGL error: " + std::to_string(error);
       }
   }

    template<typename F>
    auto glCallImpl(F&& func, const char* expr) {
    while(glGetError() != GL_NO_ERROR);
    
    if constexpr (std::is_void_v<std::invoke_result_t<F>>) {
        func();
        while(GLenum error = glGetError()) {
            OVEN_CORE_ERROR("{} in {}", getOpenGLErrorMessage(error), expr);
            OVEN_CORE_ASSERT(false, "OpenGL Error");
        }
    } else {
        auto __ret = func();
        while(GLenum error = glGetError()) {
            OVEN_CORE_ERROR("{} in {}", getOpenGLErrorMessage(error), expr);
            OVEN_CORE_ASSERT(false, "OpenGL Error");
        }
        return __ret;
    }
}



#define GL_CALL(x) glCallImpl([&](){ return x; }, #x)
#else

    #define GL_CALL(x) x

#endif