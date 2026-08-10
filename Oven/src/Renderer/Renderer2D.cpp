#include "Oven/ovenpch.h"
#include "Oven/Renderer/Renderer2D.h"
#include "Oven/Renderer/VertexArray.h"
#include "Oven/Renderer/Shader.h"
#include "Oven/Renderer/RenderCommand.h"
#include "Oven/Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
namespace Oven{

    

    struct QuadVertex{
        glm::vec3 Position; 
        glm::vec4 Color;
        glm::vec2 UV;
    };

    struct Renderer2DData{
        
        const uint32_t MaxQuadsPerCall = 10000;
        const uint32_t MaxVerticesPerCall = MaxQuadsPerCall * 4;
        const uint32_t MaxIndicesPerCall = MaxQuadsPerCall * 6;

        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<Shader> TextureShader;
        Ref<Texture2D> WhiteTexture;
        GLint TextureSlot; 
        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;
    };
    static Renderer2DData s_Data;

    

    void Renderer2D::Init(){
        
        OVEN_PROFILE_FUNCTION();

        s_Data.QuadVertexArray = Oven::VertexArray::Create();

        s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVerticesPerCall * sizeof(QuadVertex));
        s_Data.QuadVertexBuffer->SetLayout({{Oven::ShaderDataType::Float3, "a_Position"}, {Oven::ShaderDataType::Float4, "a_Color"}, {Oven::ShaderDataType::Float2, "a_TexUV"}});
        s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
        
        s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVerticesPerCall];

        uint32_t* quadIndices = new uint32_t[s_Data.MaxIndicesPerCall];

        uint32_t offset = 0;
        for(uint32_t i = 0; i < s_Data.MaxIndicesPerCall; i+=6){

            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }

        Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndicesPerCall);
        s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndices;
        
        s_Data.TextureShader = Shader::Create("sandbox/assets/shaders/Texture.glsl");
        s_Data.TextureSlot = 0; 
        s_Data.TextureShader->Bind();
        //s_Data.TextureShader->SetInt("u_Texture", s_Data.TextureSlot);

        s_Data.WhiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTexData = 0xffffffff;
        s_Data.WhiteTexture->SetData(&whiteTexData, sizeof(uint32_t));

        
    }

    void Renderer2D::Shutdown(){
        OVEN_PROFILE_FUNCTION();

    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera){
        OVEN_PROFILE_FUNCTION();

        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
        
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
    }

    void Renderer2D::EndScene(){
        OVEN_PROFILE_FUNCTION();

        uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*) s_Data.QuadVertexBufferBase;
        s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
        Flush();
    }
    void Renderer2D::Flush()
    {
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
        s_Data.QuadIndexCount = 0;
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, glm::vec2 tilingFactor)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, s_Data.WhiteTexture, color, tilingFactor);
    }
    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, glm::vec2 tilingFactor)
    {
        DrawQuad(position, size, s_Data.WhiteTexture, color, tilingFactor);
    }
    void Renderer2D::DrawQuad(const glm::vec2 & position, const glm::vec2 &size, const Ref<Texture2D> &texture, const glm::vec4 &color, glm::vec2 tilingFactor)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, texture, color, tilingFactor);
    }
    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const glm::vec4 &color, glm::vec2 tilingFactor)
    { 
        OVEN_PROFILE_FUNCTION();
        
        s_Data.QuadVertexBufferPtr->Position = position;
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->UV = {0.0f, 0.0f};
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = {position.x + size.x, position.y, 0};
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->UV = {1.0f, 0.0f};
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = {position.x + size.x, position.y + size.y, 0};;
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->UV = {1.0f, 1.0f};
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = {position.x, position.y + size.y, 0};
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->UV = {0.0f, 1.0f};
        s_Data.QuadVertexBufferPtr++;
        
        s_Data.QuadIndexCount += 6;
        /*s_Data.TextureShader->SetFloat2("u_TilingFactor", tilingFactor);
        
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        s_Data.TextureShader->SetMat4("u_Model", transform);
        texture->Bind(s_Data.TextureSlot); 
        s_Data.QuadVertexArray->Bind(); 
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray);*/

    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color, glm::vec2 tilingFactor){
        DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, texture, color, tilingFactor);
    }
    
    void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, const glm::vec4 &color, glm::vec2 tilingFactor)
    { 
        OVEN_PROFILE_FUNCTION();

        s_Data.TextureShader->SetFloat2("u_TilingFactor", glm::vec2(3.0f));
        s_Data.TextureShader->SetFloat4("u_Color", color);
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f}) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        s_Data.TextureShader->SetMat4("u_Model", transform);
        texture->Bind(s_Data.TextureSlot); 
        s_Data.QuadVertexArray->Bind(); 
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
    }


}