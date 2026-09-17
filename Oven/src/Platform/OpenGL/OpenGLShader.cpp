#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLShader.h"
#include "Oven/Core/Log.h"
#include "Oven/Core/Time.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"
#include "Oven/Platform/OpenGL/OpenGLCaps.h"

#include <cstring>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

namespace Oven
{

namespace Utils
{

// ---------------------------------------------------------------------------
// Options de compilation
//
// Ces constantes entrent dans le hash du nom de fichier de cache. Changer
// l'une d'elles invalide automatiquement les caches existants.
// ---------------------------------------------------------------------------

static constexpr shaderc_optimization_level kVulkanOptLevel = shaderc_optimization_level_zero;

// À INCRÉMENTER À LA MAIN dès que la logique de génération change
// (nouvelle option spirv-cross, post-traitement du texte, etc.).
// Sans ça, un cache produit par l'ancienne logique serait relu tel quel.
static constexpr uint32_t kCacheFormatVersion = 1;

static GLenum ShaderTypeFromString(const std::string& type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;

    OVEN_CORE_ASSERT(false, "Unknown shader type!");
    return 0;
}

static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
{
    switch (stage)
    {
        case GL_VERTEX_SHADER:
            return shaderc_glsl_vertex_shader;
        case GL_FRAGMENT_SHADER:
            return shaderc_glsl_fragment_shader;
    }
    OVEN_CORE_ASSERT(false, "");
    return (shaderc_shader_kind)0;
}

static const char* GLShaderStageToString(GLenum stage)
{
    switch (stage)
    {
        case GL_VERTEX_SHADER:
            return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER:
            return "GL_FRAGMENT_SHADER";
    }
    OVEN_CORE_ASSERT(false, "");
    return "GL_UNKNOWN_SHADER";
}

static const char* GLShaderStageFileSuffix(GLenum stage)
{
    switch (stage)
    {
        case GL_VERTEX_SHADER:
            return "vert";
        case GL_FRAGMENT_SHADER:
            return "frag";
    }
    OVEN_CORE_ASSERT(false, "");
    return "unknown";
}

static const char* GetCacheDirectory()
{
    // TODO: make sure the assets directory is valid
    return "Oven/assets/cache/shader/opengl";
}

static void CreateCacheDirectoryIfNeeded()
{
    std::string cacheDirectory = GetCacheDirectory();
    if (!std::filesystem::exists(cacheDirectory))
        std::filesystem::create_directories(cacheDirectory);
}

// FNV-1a 64 bits. Suffisant ici : on ne cherche pas de résistance
// cryptographique, juste à distinguer deux jeux d'options.
static uint64_t HashCombine(uint64_t seed, uint64_t value)
{
    seed ^= value;
    seed *= 1099511628211ull;
    return seed;
}

static std::string HashToHex(uint64_t hash)
{
    static const char* digits = "0123456789abcdef";
    std::string out(16, '0');
    for (int i = 15; i >= 0; i--)
    {
        out[i] = digits[hash & 0xF];
        hash >>= 4;
    }
    return out;
}

// Signature des options ayant servi à produire le SPIR-V canonique.
static uint64_t VulkanOptionsHash()
{
    uint64_t h = 1469598103934665603ull;
    h = HashCombine(h, (uint64_t)shaderc_env_version_vulkan_1_2);
    h = HashCombine(h, (uint64_t)kVulkanOptLevel);
    h = HashCombine(h, (uint64_t)kCacheFormatVersion);
    return h;
}

// Signature des options ayant servi à produire la sortie plateforme.
static uint64_t OpenGLOptionsHash(bool spirvPath, uint32_t glslVersion)
{
    uint64_t h = 1469598103934665603ull;
    h = HashCombine(h, spirvPath ? 1ull : 0ull);
    h = HashCombine(h, (uint64_t)glslVersion);
    h = HashCombine(h, (uint64_t)(glslVersion >= 420)); // enable_420pack_extension
    h = HashCombine(h, 1ull);                           // separate_shader_objects
    h = HashCombine(h, (uint64_t)kCacheFormatVersion);
    return h;
}

} // namespace Utils

// ---------------------------------------------------------------------------
// Cache
// ---------------------------------------------------------------------------

std::filesystem::path OpenGLShader::GetCachedPath(GLenum stage, bool vulkanStage) const
{
    std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();
    std::string filename = std::filesystem::path(m_FilePath).filename().string();

    if (vulkanStage)
    {
        // ex: Texture.glsl.vk.3a7f1c92d04be115.vert
        return cacheDirectory / (filename + ".vk." + Utils::HashToHex(Utils::VulkanOptionsHash()) + "." +
                                 Utils::GLShaderStageFileSuffix(stage));
    }

    const bool spirvPath = OpenGLCaps::SupportsSpirv();
    const uint32_t version = OpenGLCaps::GLSLVersion();

    // ex: Texture.glsl.gl410.glsl.b91e04ac7f22d316.frag
    return cacheDirectory / (filename + ".gl" + std::to_string(version) + (spirvPath ? ".spv." : ".glsl.") +
                             Utils::HashToHex(Utils::OpenGLOptionsHash(spirvPath, version)) + "." +
                             Utils::GLShaderStageFileSuffix(stage));
}

bool OpenGLShader::CanUseCache(const std::filesystem::path& cachedPath) const
{
    if (!m_CacheEnabled || s_ForceRecompile)
        return false;

    std::error_code ec;
    if (!std::filesystem::exists(cachedPath, ec) || ec)
        return false;

    auto cacheTime = std::filesystem::last_write_time(cachedPath, ec);
    if (ec)
        return false;

    auto sourceTime = std::filesystem::last_write_time(m_FilePath, ec);
    if (ec)
        return false; // source illisible : on préfère recompiler

    // Le cache n'est valide que s'il est postérieur à la dernière édition du shader.
    return sourceTime <= cacheTime;
}

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

OpenGLShader::OpenGLShader(const std::string& filepath) : m_FilePath(filepath)
{
    OVEN_PROFILE_FUNCTION();

    Utils::CreateCacheDirectoryIfNeeded();

    // Le nom d'abord : les logs de compilation s'en servent.
    auto lastSlash = filepath.find_last_of("/\\");
    lastSlash = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
    auto lastDot = filepath.rfind('.');
    auto count = (lastDot == std::string::npos) ? filepath.size() - lastSlash : lastDot - lastSlash;
    m_Name = filepath.substr(lastSlash, count);

    std::string source = ReadFile(filepath);
    if (source.empty())
        return;

    auto shaderSources = PreProcess(source);

    float time = Time::GetTime();
    CompileOrGetVulkanBinaries(shaderSources);
    CompileOrGetOpenGLBinaries();
    CreateProgram();
    OVEN_CORE_INFO("Shader '{0}' prêt en {1:.2f} ms", m_Name, (Time::GetTime() - time) * 1000.0f);
}

OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    : m_Name(name)
{
    OVEN_PROFILE_FUNCTION();

    // Sources fournies en mémoire : aucun fichier dont comparer la date,
    // donc pas de cache disque pour ce shader.
    m_FilePath = name;
    m_CacheEnabled = false;

    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSrc;
    sources[GL_FRAGMENT_SHADER] = fragmentSrc;

    CompileOrGetVulkanBinaries(sources);
    CompileOrGetOpenGLBinaries();
    CreateProgram();
}

OpenGLShader::~OpenGLShader()
{
    OVEN_PROFILE_FUNCTION();

    if (m_RendererID != 0)
        GL_CALL(glDeleteProgram(m_RendererID));
}

// ---------------------------------------------------------------------------
// Lecture et découpage
// ---------------------------------------------------------------------------

std::string OpenGLShader::ReadFile(const std::string& filepath)
{
    OVEN_PROFILE_FUNCTION();

    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary); // fermé par RAII
    if (in)
    {
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        if (size > 0)
        {
            result.resize(size);
            in.seekg(0, std::ios::beg);
            in.read(&result[0], size);
        }
        else
        {
            OVEN_CORE_ERROR("Could not read from file '{0}'", filepath);
        }
    }
    else
    {
        OVEN_CORE_ERROR("Could not open file '{0}'", filepath);
    }

    return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
{
    OVEN_PROFILE_FUNCTION();

    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "#type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);
    while (pos != std::string::npos)
    {
        size_t eol = source.find_first_of("\r\n", pos);
        OVEN_CORE_ASSERT(eol != std::string::npos, "Syntax error");
        size_t begin = pos + typeTokenLength + 1;
        std::string type = source.substr(begin, eol - begin);
        OVEN_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified");

        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        OVEN_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
        pos = source.find(typeToken, nextLinePos);

        shaderSources[Utils::ShaderTypeFromString(type)] =
            (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
    }

    return shaderSources;
}

// ---------------------------------------------------------------------------
// Étape 1 : GLSL 450 -> SPIR-V canonique
// ---------------------------------------------------------------------------

void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
{
    OVEN_PROFILE_FUNCTION();

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

    // ATTENTION : les niveaux d'optimisation au-dessus de zero suppriment les
    // instructions OpName du binaire. Sans elles, la réflexion ne renvoie plus
    // que des identifiants générés (_47_49) et ApplyResourceBindings ne peut
    // plus retrouver les blocs par nom — ce qui est indispensable en GL 4.1.
    options.SetOptimizationLevel(Utils::kVulkanOptLevel);

    auto& shaderData = m_VulkanSPIRV;
    shaderData.clear();
    m_UniformBuffers.clear();
    m_SampledImages.clear();

    for (auto&& [stage, source] : shaderSources)
    {
        std::filesystem::path cachedPath = GetCachedPath(stage, true);

        if (CanUseCache(cachedPath))
        {
            std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
            if (in.is_open())
            {
                in.seekg(0, std::ios::end);
                auto size = in.tellg();
                in.seekg(0, std::ios::beg);

                auto& data = shaderData[stage];
                data.resize(size / sizeof(uint32_t));
                in.read(reinterpret_cast<char*>(data.data()), size);
                continue;
            }
        }

        shaderc::SpvCompilationResult module =
            compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);

        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            OVEN_CORE_ERROR("Vulkan Shader Compilation Error ({0}):\n{1}", m_FilePath, module.GetErrorMessage());
            OVEN_CORE_ASSERT(false, "");
            return;
        }

        shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

        if (m_CacheEnabled)
        {
            std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
            if (out.is_open())
            {
                auto& data = shaderData[stage];
                out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(uint32_t));
                out.flush();
            }
        }
    }

    for (auto&& [stage, data] : shaderData)
        Reflect(stage, data);
}

// ---------------------------------------------------------------------------
// Étape 2 : SPIR-V canonique -> sortie plateforme
// ---------------------------------------------------------------------------

void OpenGLShader::CompileOrGetOpenGLBinaries()
{
    OVEN_PROFILE_FUNCTION();

    const bool spirvPath = OpenGLCaps::SupportsSpirv();
    const uint32_t version = OpenGLCaps::GLSLVersion();

    m_OpenGLSPIRV.clear();
    m_OpenGLSourceCode.clear();

    for (auto&& [stage, spirv] : m_VulkanSPIRV)
    {
        std::filesystem::path cachedPath = GetCachedPath(stage, false);

        // ---------- Lecture du cache ----------
        if (CanUseCache(cachedPath))
        {
            if (spirvPath)
            {
                std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
                if (in.is_open())
                {
                    in.seekg(0, std::ios::end);
                    auto size = in.tellg();
                    in.seekg(0, std::ios::beg);

                    auto& data = m_OpenGLSPIRV[stage];
                    data.resize(size / sizeof(uint32_t));
                    in.read(reinterpret_cast<char*>(data.data()), size);
                    continue;
                }
            }
            else
            {
                std::ifstream in(cachedPath, std::ios::in);
                if (in.is_open())
                {
                    std::stringstream ss;
                    ss << in.rdbuf();
                    m_OpenGLSourceCode[stage] = ss.str();
                    continue;
                }
            }
        }

        // ---------- SPIR-V -> texte GLSL ----------
        spirv_cross::CompilerGLSL glslCompiler(spirv.data(), spirv.size());

        spirv_cross::CompilerGLSL::Options glslOptions;
        glslOptions.version = version;
        glslOptions.es = false;

        // layout(binding=) vient de ARB_shading_language_420pack (GLSL 420).
        // En dessous, spirv-cross le retire et ApplyResourceBindings le rétablit.
        glslOptions.enable_420pack_extension = (version >= 420);

        // Sans ça, spirv-cross n'émet pas les layout(location=) sur les varyings.
        // Les deux stages étant compilés séparément, l'appariement retomberait
        // alors sur des noms générés qui ne correspondent pas d'un stage à l'autre.
        glslOptions.separate_shader_objects = true;

        glslCompiler.set_common_options(glslOptions);

        std::string source = glslCompiler.compile();

        // separate_shader_objects force la redéclaration du bloc gl_PerVertex,
        // et spirv-cross y met gl_CullDistance, qui n'existe qu'à partir de
        // GL 4.5 (ARB_cull_distance). Contournement textuel, donc fragile :
        // à revérifier après toute mise à jour de spirv-cross.
        if (version < 450)
        {
            size_t p = source.find("gl_CullDistance");
            if (p != std::string::npos)
            {
                size_t lineStart = source.rfind('\n', p);
                size_t lineEnd = source.find('\n', p);
                if (lineStart != std::string::npos && lineEnd != std::string::npos)
                    source.erase(lineStart, lineEnd - lineStart);
            }
        }

        m_OpenGLSourceCode[stage] = source;

        // ---------- Chemin GLSL texte : terminé ----------
        if (!spirvPath)
        {
            if (m_CacheEnabled)
            {
                std::ofstream out(cachedPath, std::ios::out);
                if (out.is_open())
                    out << source;
            }
            continue;
        }

        // ---------- Chemin SPIR-V : retour au binaire ----------
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);

        shaderc::SpvCompilationResult module =
            compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);

        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            OVEN_CORE_ERROR("OpenGL Shader Compilation Error ({0}):\n{1}", m_FilePath, module.GetErrorMessage());
            OVEN_CORE_ASSERT(false, "");
            continue;
        }

        m_OpenGLSPIRV[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

        if (m_CacheEnabled)
        {
            std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
            if (out.is_open())
            {
                auto& data = m_OpenGLSPIRV[stage];
                out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(uint32_t));
                out.flush();
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Étape 3 : upload et link
// ---------------------------------------------------------------------------

void OpenGLShader::CreateProgram()
{
    OVEN_PROFILE_FUNCTION();

    GLuint program = GL_CALL(glCreateProgram());
    std::vector<GLuint> shaderIDs;
    bool compileFailed = false;

    if (OpenGLCaps::SupportsSpirv())
    {
        for (auto&& [stage, spirv] : m_OpenGLSPIRV)
        {
            GLuint shaderID = shaderIDs.emplace_back(GL_CALL(glCreateShader(stage)));
            GL_CALL(glShaderBinary(
                1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), (GLsizei)(spirv.size() * sizeof(uint32_t))
            ));
            GL_CALL(glSpecializeShader(shaderID, "main", 0, nullptr, nullptr));

            GLint ok = GL_FALSE;
            GL_CALL(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &ok));
            if (ok == GL_FALSE)
            {
                GLint len = 0;
                GL_CALL(glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len));
                std::vector<GLchar> log(len > 0 ? len : 1);
                GL_CALL(glGetShaderInfoLog(shaderID, len, &len, log.data()));

                OVEN_CORE_ERROR(
                    "SPIR-V specialization failed — {0} ({1}):\n{2}",
                    Utils::GLShaderStageToString(stage),
                    m_FilePath,
                    log.data()
                );
                compileFailed = true;
            }

            GL_CALL(glAttachShader(program, shaderID));
        }
    }
    else
    {
        for (auto&& [stage, source] : m_OpenGLSourceCode)
        {
            GLuint shaderID = shaderIDs.emplace_back(GL_CALL(glCreateShader(stage)));
            const char* src = source.c_str();
            GL_CALL(glShaderSource(shaderID, 1, &src, nullptr));
            GL_CALL(glCompileShader(shaderID));

            GLint ok = GL_FALSE;
            GL_CALL(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &ok));
            if (ok == GL_FALSE)
            {
                GLint len = 0;
                GL_CALL(glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len));
                std::vector<GLchar> log(len > 0 ? len : 1);
                GL_CALL(glGetShaderInfoLog(shaderID, len, &len, log.data()));

                OVEN_CORE_ERROR(
                    "GLSL compile failed — {0} ({1}):\n{2}", Utils::GLShaderStageToString(stage), m_FilePath, log.data()
                );
                OVEN_CORE_ERROR("Source générée :\n{0}", source);
                compileFailed = true;
            }

            GL_CALL(glAttachShader(program, shaderID));
        }
    }

    // Inutile de linker si un stage n'a pas compilé : le log de link ne dirait
    // que "one or more attached shaders not successfully compiled" et noierait
    // l'erreur utile au-dessus.
    if (compileFailed)
    {
        for (auto id : shaderIDs)
        {
            GL_CALL(glDetachShader(program, id));
            GL_CALL(glDeleteShader(id));
        }
        GL_CALL(glDeleteProgram(program));

        m_RendererID = 0;
        OVEN_CORE_ASSERT(false, "Shader compilation failed");
        return;
    }

    GL_CALL(glLinkProgram(program));

    GLint isLinked = GL_FALSE;
    GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, &isLinked));
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        GL_CALL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength));

        std::vector<GLchar> infoLog(maxLength > 0 ? maxLength : 1);
        GL_CALL(glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data()));
        OVEN_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, infoLog.data());

        for (auto id : shaderIDs)
        {
            GL_CALL(glDetachShader(program, id));
            GL_CALL(glDeleteShader(id));
        }
        GL_CALL(glDeleteProgram(program));

        m_RendererID = 0;
        OVEN_CORE_ASSERT(false, "Shader link failed");
        return;
    }

    for (auto id : shaderIDs)
    {
        GL_CALL(glDetachShader(program, id));
        GL_CALL(glDeleteShader(id));
    }

    m_RendererID = program;

    ApplyResourceBindings();
}

// ---------------------------------------------------------------------------
// Bindings manuels
// ---------------------------------------------------------------------------

void OpenGLShader::ApplyResourceBindings()
{
    OVEN_PROFILE_FUNCTION();

    if (m_RendererID == 0)
        return;

    // Uniform blocks. En GL 4.1 il n'existe aucun moyen de désigner un bloc
    // autrement que par son nom, d'où la dépendance à la réflexion.
    for (const auto& ubo : m_UniformBuffers)
    {
        GLuint index = GL_CALL(glGetUniformBlockIndex(m_RendererID, ubo.name.c_str()));
        if (index != GL_INVALID_INDEX)
        {
            GL_CALL(glUniformBlockBinding(m_RendererID, index, ubo.binding));
        }
        else
        {
            OVEN_CORE_WARN("Uniform block '{0}' introuvable dans le programme lié ({1})", ubo.name, m_FilePath);
        }
    }

    // Samplers. Un sampler ne se "binde" pas : on lui assigne un numéro
    // d'unité de texture via glUniform1i.
    GL_CALL(glUseProgram(m_RendererID));
    for (const auto& sampler : m_SampledImages)
    {
        GLint location = GL_CALL(glGetUniformLocation(m_RendererID, sampler.name.c_str()));
        if (location == -1)
        {
            OVEN_CORE_WARN("Sampler '{0}' introuvable dans le programme lié ({1})", sampler.name, m_FilePath);
            continue;
        }

        // Un tableau de samplers reçoit des unités consécutives depuis son binding.
        GLint arraySize = 1;
        {
            GLuint uniformIndex = GL_INVALID_INDEX;
            const char* namePtr = sampler.name.c_str();
            GL_CALL(glGetUniformIndices(m_RendererID, 1, &namePtr, &uniformIndex));
            if (uniformIndex != GL_INVALID_INDEX)
                GL_CALL(glGetActiveUniformsiv(m_RendererID, 1, &uniformIndex, GL_UNIFORM_SIZE, &arraySize));
        }

        if (arraySize < 1)
            arraySize = 1;

        std::vector<GLint> units(arraySize);
        for (GLint i = 0; i < arraySize; i++)
            units[i] = (GLint)sampler.binding + i;

        GL_CALL(glUniform1iv(location, arraySize, units.data()));
    }
    GL_CALL(glUseProgram(0));
}

// ---------------------------------------------------------------------------
// Réflexion
// ---------------------------------------------------------------------------

void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
{
    OVEN_PROFILE_FUNCTION();

    if (shaderData.empty())
        return;

    spirv_cross::Compiler compiler(shaderData.data(), shaderData.size());
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    OVEN_CORE_TRACE("Reflect {0} — {1}", Utils::GLShaderStageToString(stage), m_FilePath);

    for (const auto& resource : resources.uniform_buffers)
    {
        const auto& bufferType = compiler.get_type(resource.base_type_id);

        UniformBufferInfo info;

        // resource.name est le nom de l'INSTANCE du bloc (u_Camera).
        // glGetUniformBlockIndex attend le nom du BLOC (Camera), porté par base_type_id.
        info.name = compiler.get_name(resource.base_type_id);
        if (info.name.empty())
            info.name = resource.name;

        info.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
        info.size = (uint32_t)compiler.get_declared_struct_size(bufferType);

        OVEN_CORE_TRACE("  UBO '{0}' — size {1}, binding {2}", info.name, info.size, info.binding);

        // Le même bloc apparaît dans plusieurs stages : une seule entrée suffit.
        bool alreadyKnown = false;
        for (const auto& existing : m_UniformBuffers)
        {
            if (existing.name == info.name)
            {
                alreadyKnown = true;
                break;
            }
        }

        if (!alreadyKnown)
            m_UniformBuffers.push_back(info);
    }

    for (const auto& resource : resources.sampled_images)
    {
        SampledImageInfo info;
        info.name = resource.name;
        info.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

        OVEN_CORE_TRACE("  Sampler '{0}' — binding {1}", info.name, info.binding);

        bool alreadyKnown = false;
        for (const auto& existing : m_SampledImages)
        {
            if (existing.name == info.name)
            {
                alreadyKnown = true;
                break;
            }
        }

        if (!alreadyKnown)
            m_SampledImages.push_back(info);
    }
}

// ---------------------------------------------------------------------------
// Bind / uniforms
// ---------------------------------------------------------------------------

void OpenGLShader::Bind() const
{
    OVEN_PROFILE_FUNCTION();

    GL_CALL(glUseProgram(m_RendererID));
}

void OpenGLShader::Unbind() const
{
    OVEN_PROFILE_FUNCTION();

    GL_CALL(glUseProgram(0));
}

void OpenGLShader::SetInt(const std::string& name, int value)
{
    OVEN_PROFILE_FUNCTION();

    UploadUniformInt(name, value);
}

void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
{
    OVEN_PROFILE_FUNCTION();

    UploadUniformIntArray(name, values, count);
}

void OpenGLShader::SetFloat(const std::string& name, float value)
{
    OVEN_PROFILE_FUNCTION();

    UploadUniformFloat(name, value);
}

void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
{
    OVEN_PROFILE_FUNCTION();

    UploadUniformFloat2(name, value);
}

void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
{
    OVEN_PROFILE_FUNCTION();

    UploadUniformFloat3(name, value);
}

void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
{
    OVEN_PROFILE_FUNCTION();

    UploadUniformFloat4(name, value);
}

void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
{
    OVEN_PROFILE_FUNCTION();

    UploadUniformMat4(name, value);
}

void OpenGLShader::UploadUniformInt(const std::string& name, int value)
{
    GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
    GL_CALL(glUniform1i(location, value));
}

void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
{
    GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
    GL_CALL(glUniform1iv(location, count, values));
}

void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
{
    GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
    GL_CALL(glUniform1f(location, value));
}

void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
{
    GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
    GL_CALL(glUniform2f(location, value.x, value.y));
}

void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
{
    GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
    GL_CALL(glUniform3f(location, value.x, value.y, value.z));
}

void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
{
    GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
    GL_CALL(glUniform4f(location, value.x, value.y, value.z, value.w));
}

void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
{
    GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
    GL_CALL(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
    GLint location = GL_CALL(glGetUniformLocation(m_RendererID, name.c_str()));
    GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

} // namespace Oven