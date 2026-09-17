#pragma once

#include "Oven/Renderer/Shader.h"
#include <glm/glm.hpp>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace Oven
{

class OpenGLShader : public Shader
{
public:
    OpenGLShader(const std::string& filepath);
    OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    virtual ~OpenGLShader();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void SetInt(const std::string& name, int value) override;
    virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
    virtual void SetFloat(const std::string& name, float value) override;
    virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
    virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
    virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
    virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

    virtual const std::string& GetDefaultName() const override { return m_Name; }

    // Ignore le cache disque pour toutes les compilations suivantes.
    // À brancher sur un argument de ligne de commande ou un bouton de l'éditeur.
    static void SetForceRecompile(bool force) { s_ForceRecompile = force; }
    static bool GetForceRecompile() { return s_ForceRecompile; }

    void UploadUniformInt(const std::string& name, int value);
    void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

    void UploadUniformFloat(const std::string& name, float value);
    void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
    void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
    void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

    void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
    void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

private:
    // Métadonnées extraites du SPIR-V par réflexion, conservées pour le binding manuel
    // que réclame OpenGL 4.1 (où layout(binding=) n'existe pas).
    struct UniformBufferInfo
    {
        std::string name;
        uint32_t binding = 0;
        uint32_t size = 0;
    };

    struct SampledImageInfo
    {
        std::string name;
        uint32_t binding = 0;
    };

private:
    std::string ReadFile(const std::string& filepath);
    std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);

    void CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);
    void CompileOrGetOpenGLBinaries();
    void CreateProgram();
    void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData);

    // Réinjecte les bindings découverts par réflexion. Sans effet si le shader
    // les déclarait déjà (GLSL >= 420), donc appelable inconditionnellement.
    void ApplyResourceBindings();

    // Chemin du fichier de cache pour un stage donné.
    // vulkanStage = true  -> SPIR-V canonique (sortie de shaderc)
    // vulkanStage = false -> sortie plateforme (SPIR-V OpenGL ou texte GLSL)
    std::filesystem::path GetCachedPath(GLenum stage, bool vulkanStage) const;

    // true si le cache est utilisable : il existe, il est plus récent que la
    // source, et le shader n'a pas été construit depuis des sources inline.
    bool CanUseCache(const std::filesystem::path& cachedPath) const;

private:
    uint32_t m_RendererID = 0;
    std::string m_FilePath;
    std::string m_Name;

    // Les shaders construits depuis des chaînes en mémoire ne sont pas mis en
    // cache : il n'y a pas de fichier source dont comparer la date.
    bool m_CacheEnabled = true;

    std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRV;
    std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRV;

    std::unordered_map<GLenum, std::string> m_OpenGLSourceCode;

    std::vector<UniformBufferInfo> m_UniformBuffers;
    std::vector<SampledImageInfo> m_SampledImages;

    inline static bool s_ForceRecompile = false;
};

} // namespace Oven