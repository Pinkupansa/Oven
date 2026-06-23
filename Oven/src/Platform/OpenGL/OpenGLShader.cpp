#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLShader.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include "Oven/Log.h"
#include <glad/glad.h>
namespace Oven
{

    static GLenum StringToShaderType(const std::string& type){
        if(type == "vertex") return GL_VERTEX_SHADER;
        if(type == "fragment" or type == "pixel") return GL_FRAGMENT_SHADER;

        OVEN_CORE_ERROR("Unknown shader type : {0} !", type);
        OVEN_CORE_ASSERT(false, "Shutting down...");
    }
    OpenGLShader::OpenGLShader(const std::string &filepath){
        std::string source = ReadFile(filepath);
        auto shaderSources = SplitShaderSources(source);
        Compile(shaderSources);
    }
    OpenGLShader::OpenGLShader(const std::string &vertexSrc, const std::string &fragmentSrc)
    {
        std::unordered_map<GLenum, std::string> sources; 
        sources[GL_VERTEX_SHADER] = vertexSrc; 
        sources[GL_FRAGMENT_SHADER] = fragmentSrc; 
        Compile(sources);
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources){
        GLuint program = GL_CALL(glCreateProgram());
        std::vector<GLenum> glShaderIds;
        for(auto& kv : shaderSources){
            GLenum shaderType = kv.first; 
            const std::string& shaderSource = kv.second;
            GLuint shader = GL_CALL(glCreateShader(shaderType));

            // Note that std::string's .c_str is NULL character terminated.
            const GLchar *source = shaderSource.c_str();
            GL_CALL(glShaderSource(shader, 1, &source, 0));

            // Compile the vertex shader
            GL_CALL(glCompileShader(shader));

            GLint isCompiled = 0;
            GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled));
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                GL_CALL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength));

                // The maxLength includes the NULL character
                std::vector<GLchar> infoLog(maxLength);
                GL_CALL(glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]));

                // We don't need the shader anymore.
                GL_CALL(glDeleteShader(shader));
                
                OVEN_CORE_ERROR("Shader compilation failed ! Message : {0}", infoLog.data());
                OVEN_CORE_ASSERT(false, "Shutting down...");
            }
            GL_CALL(glAttachShader(program, shader));
            glShaderIds.push_back(shader);
        }

        
        m_RendererID = program;
        // Attach our shaders to our program

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
            for(auto id : glShaderIds){
                GL_CALL(glDeleteShader(id));
            }
            
            OVEN_CORE_ERROR("Shader linking failed ! Message : {0}", infoLog.data());
            OVEN_CORE_ASSERT(false, "Shutting down...");
            // In this simple program, we'll just leave
            return;
        }

        for(auto id : glShaderIds){
            GL_CALL(glDetachShader(program, id));
        }
    }
    std::string OpenGLShader::ReadFile(const std::string& filepath){
        std::ifstream in(filepath, std::ios::in | std::ios::binary);

        std::string result;
        if(in){
            in.seekg(0, std::ios::end);
            result.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&result[0], result.size());
            in.close();
        }
        else{
            OVEN_CORE_ERROR("Could not open file {0} !", filepath);
            OVEN_CORE_ASSERT(false, "Shutting down...");
            return result;
        }
        return result;
    }

    OpenGLShader::~OpenGLShader(){
        GL_CALL(glDeleteProgram(m_RendererID));
    }

    std::unordered_map<GLenum, std::string> OpenGLShader::SplitShaderSources(const std::string& source){
        std::unordered_map<GLenum, std::string> shaderSources; 
        const char* typeMarker = "#type";
        size_t typeMarkerLength = strlen(typeMarker);
        size_t pos = source.find(typeMarker, 0);

        while(pos != std::string::npos){
            size_t lineEnd = source.find_first_of("\r\n", pos);
            OVEN_CORE_ASSERT(lineEnd != std::string::npos, "Syntax error");
            size_t typeNameStart = pos + typeMarkerLength + 1; 
            std::string shaderType = source.substr(typeNameStart, lineEnd - typeNameStart);
            OVEN_CORE_ASSERT(shaderType == "vertex" || shaderType == "fragment" || shaderType == "pixel", "Unknown shader type !");
            size_t firstSourceLinePos = source.find_first_not_of("\r\n", lineEnd);
            pos = source.find(typeMarker, firstSourceLinePos);
            shaderSources[StringToShaderType(shaderType)] = source.substr(firstSourceLinePos, pos - firstSourceLinePos);
        }
        return shaderSources;
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