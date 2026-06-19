#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLShader.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include "Oven/Log.h"

namespace Oven
{

    OpenGLShader::OpenGLShader(const std::string &vertexSrc, const std::string &fragmentSrc)
    {
        GLuint vertexShader = GL_CALL(glCreateShader(GL_VERTEX_SHADER));

        // Send the vertex shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        const GLchar *source = vertexSrc.c_str();
        GL_CALL(glShaderSource(vertexShader, 1, &source, 0));

        // Compile the vertex shader
        GL_CALL(glCompileShader(vertexShader));

        GLint isCompiled = 0;
        GL_CALL(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled));
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            GL_CALL(glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength));

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            GL_CALL(glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]));

            // We don't need the shader anymore.
            GL_CALL(glDeleteShader(vertexShader));
            
            OVEN_CORE_ERROR("Vertex shader compilation failed ! Message : {0}", infoLog.data());
            OVEN_CORE_ASSERT(false, "Shutting down...");
        }

        // Create an empty fragment shader handle
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // Send the fragment shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        source = (const GLchar *)fragmentSrc.c_str();
        GL_CALL(glShaderSource(fragmentShader, 1, &source, 0));

        // Compile the fragment shader
        GL_CALL(glCompileShader(fragmentShader));

        GL_CALL(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled));
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            GL_CALL(glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength));

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            GL_CALL(glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]));

            // We don't need the shader anymore.
            GL_CALL(glDeleteShader(fragmentShader));
            // Either of them. Don't leak shaders.
            GL_CALL(glDeleteShader(vertexShader));

            OVEN_CORE_ERROR("Fragment shader compilation failed ! Message : {0}", infoLog.data());
            OVEN_CORE_ASSERT(false, "Shutting down...");

            // In this simple program, we'll just leave
            return;
        }

        // Vertex and fragment shaders are successfully compiled.
        // Now time to link them together into a program.
        // Get a program object.
        
        GLuint program = GL_CALL(glCreateProgram());
        m_RendererID = program;
        // Attach our shaders to our program
        GL_CALL(glAttachShader(program, vertexShader));
        GL_CALL(glAttachShader(program, fragmentShader));

        // Link our program
        GL_CALL(glLinkProgram(program));

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked));
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            GL_CALL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength));

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            GL_CALL(glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]));

            // We don't need the program anymore.
            GL_CALL(glDeleteProgram(program));
            // Don't leak shaders either.
            GL_CALL(glDeleteShader(vertexShader));
            GL_CALL(glDeleteShader(fragmentShader));

            
            OVEN_CORE_ERROR("Shader linking failed ! Message : {0}", infoLog.data());
            OVEN_CORE_ASSERT(false, "Shutting down...");
            // In this simple program, we'll just leave
            return;
        }

        // Always detach shaders after a successful link.
        GL_CALL(glDetachShader(program, vertexShader));
        GL_CALL(glDetachShader(program, fragmentShader));
    }

    OpenGLShader::~OpenGLShader(){
        GL_CALL(glDeleteProgram(m_RendererID));
    }

    void OpenGLShader::Bind() const{
        GL_CALL(glUseProgram(m_RendererID));
    }

    void OpenGLShader::Unbind() const{
        GL_CALL(glUseProgram(0));
    }

    void OpenGLShader::UploadUniformInt(const std::string& name, int value){
        GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
        GL_CALL(glUniform1i(location, value));
    }

    void OpenGLShader::UploadUniformFloat(const std::string& name, float value){
        GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
        GL_CALL(glUniform1f(location, value));
    }


    void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values){
        GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
        GL_CALL(glUniform2f(location, values.x, values.y));
    }


    void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values){
        GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
        GL_CALL(glUniform3f(location, values.x, values.y, values.z));
    }

    void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values){
        GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
        GL_CALL(glUniform4f(location, values.x, values.y, values.z, values.w));
    }


    void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix){
        GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
        GL_CALL(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
    }
    
    void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix){
        GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
        GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
    }
}