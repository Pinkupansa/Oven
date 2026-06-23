#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLTexture.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"
#include "Oven/Log.h"
#include <glad/glad.h>
#include "vendor/stb_image/stb_image.h"

namespace Oven{

    OpenGLTexture2D::OpenGLTexture2D(const std::string &path) : m_Path(path){
        int width, height, channels;

        stbi_set_flip_vertically_on_load(1);
        stbi_uc *data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        OVEN_CORE_ASSERT(data, "Failed to load image!");
        m_Width = width;
        m_Height = height;
 
        GLenum dataFormat, internalFormat  = 0;
        if(channels == 4){
            internalFormat = GL_SRGB8_ALPHA8; 
            dataFormat = GL_RGBA;
        }
        else if(channels == 3){
            internalFormat = GL_SRGB8;
            dataFormat = GL_RGB;
        }

        OVEN_CORE_ASSERT(internalFormat && dataFormat, "Image format not supported !");

        // 1. Générer l'ID de la texture (générique en 4.1)
        GL_CALL(glGenTextures(1, &m_RendererID));

        // 2. Lier la texture à la cible GL_TEXTURE_2D pour pouvoir la configurer
        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));

        // 3. Configurer les paramètres de filtrage (on utilise glTexParameter au lieu de glTextureParameter)
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        // 4. Allouer la mémoire ET envoyer les données d'un coup avec glTexImage2D
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data));

        // 5. [Optionnel mais recommandé] Unbind la texture pour éviter les effets de bord ailleurs dans le code
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
        

        stbi_image_free(data);
    }

    OpenGLTexture2D::~OpenGLTexture2D(){
        GL_CALL(glDeleteTextures(1, &m_RendererID));
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const{
        GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));
    }
}