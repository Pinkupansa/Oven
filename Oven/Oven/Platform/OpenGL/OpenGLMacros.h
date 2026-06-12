#pragma once

#ifdef OVEN_DEBUG

    #define GLCall(x) \
        do { \
            while(glGetError() != GL_NO_ERROR); \
            x; \
            GLenum error; \
            while((error = glGetError()) != GL_NO_ERROR) { \
                OVEN_CORE_ERROR("OpenGL Error {} in {}", error, #x); \
            } \
        } while(0)

#else

    #define GLCall(x) x

#endif