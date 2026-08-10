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
        float TexIndex;
        glm::vec2 TilingFactor;
    };

    struct Renderer2DData{
        
        const uint32_t MaxQuadsPerCall = 10000;
        const uint32_t MaxVerticesPerCall = MaxQuadsPerCall * 4;
        const uint32_t MaxIndicesPerCall = MaxQuadsPerCall * 6;
        static const uint32_t MaxTextureSlots = 16; 

        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<Shader> TextureShader;
        Ref<Texture2D> WhiteTexture;
        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;

        std::array<Ref<Texture2D>, MaxTextureSlots> Textures; 
        uint32_t CurrentTextureSlotIndex = 1; //0 is white
    };
    static Renderer2DData s_Data;

    

    void Renderer2D::Init(){
        
        OVEN_PROFILE_FUNCTION();

        s_Data.QuadVertexArray = Oven::VertexArray::Create();

        s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVerticesPerCall * sizeof(QuadVertex));
        s_Data.QuadVertexBuffer->SetLayout({
              {Oven::ShaderDataType::Float3, "a_Position"},
              {Oven::ShaderDataType::Float4, "a_Color"},
              {Oven::ShaderDataType::Float2, "a_TexUV"}, 
              {Oven::ShaderDataType::Float, "a_TexIndex"},
              {Oven::ShaderDataType::Float2, "a_TilingFactor"}
            });
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
        
        

        s_Data.WhiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTexData = 0xffffffff;
        s_Data.WhiteTexture->SetData(&whiteTexData, sizeof(uint32_t));
        
        int32_t samplers[s_Data.MaxTextureSlots];
        for(uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
            samplers[i] = i;

        s_Data.TextureShader = Shader::Create("sandbox/assets/shaders/Texture.glsl");
        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

        s_Data.Textures[0] = s_Data.WhiteTexture;
        s_Data.Textures[0]->Bind(0);
    }

    void Renderer2D::Shutdown(){
        OVEN_PROFILE_FUNCTION();

    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera){
        OVEN_PROFILE_FUNCTION();

        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
        
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.CurrentTextureSlotIndex = 1;
    }

    void Renderer2D::EndScene(){
        OVEN_PROFILE_FUNCTION();

        uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*) s_Data.QuadVertexBufferBase;
        s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
        Flush();
    }
    void Renderer2D::Flush()
    {
        for(uint32_t i = 0; i < s_Data.CurrentTextureSlotIndex; i++){
            s_Data.Textures[i]->Bind(i);
        }
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
        s_Data.QuadIndexCount = 0;
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, color);
    }
    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
    { 
        OVEN_PROFILE_FUNCTION();
        const float whiteTextureIndex = 0.0f;
        const glm::vec2 tilingFactor = {1.0f, 1.0f};

        s_Data.QuadVertexBufferPtr->Position = position;
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->UV = {0.0f, 0.0f};
        s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = {position.x + size.x, position.y, 0};
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->UV = {1.0f, 0.0f};
        s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = {position.x + size.x, position.y + size.y, 0};;
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->UV = {1.0f, 1.0f};
        s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = {position.x, position.y + size.y, 0};
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->UV = {0.0f, 1.0f};
        s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;
        
        s_Data.QuadIndexCount += 6;

    }
    void Renderer2D::DrawQuad(const glm::vec2 & position, const glm::vec2 &size, const Ref<Texture2D> &texture, const glm::vec4 &color, glm::vec2 tilingFactor)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, texture, color, tilingFactor);
    }
    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const glm::vec4 &color, glm::vec2 tilingFactor)
    { 
        OVEN_PROFILE_FUNCTION();    

        float textureIndex = -1;

        for(uint32_t i = 0; i < s_Data.CurrentTextureSlotIndex; i++){
            if(*(s_Data.Textures[i].get()) == *(texture.get())){
                textureIndex = (float)i;
                break;
            }
        }

        if(textureIndex < 0){
            textureIndex = (float)s_Data.CurrentTextureSlotIndex;
            s_Data.Textures[s_Data.CurrentTextureSlotIndex] = texture;
            s_Data.CurrentTextureSlotIndex ++;
        }
        
        s_Data.QuadVertexBufferPtr->Position = position;
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->UV = {0.0f, 0.0f};
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = {position.x + size.x, position.y, 0};
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->UV = {1.0f, 0.0f};
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = {position.x + size.x, position.y + size.y, 0};;
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->UV = {1.0f, 1.0f};
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = {position.x, position.y + size.y, 0};
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->UV = {0.0f, 1.0f};
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;
        
        s_Data.QuadIndexCount += 6;

    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color, glm::vec2 tilingFactor){
        DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, texture, color, tilingFactor);
    }
    
    void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, const glm::vec4 &color, glm::vec2 tilingFactor)
    { 
        OVEN_PROFILE_FUNCTION(); 
    }


}