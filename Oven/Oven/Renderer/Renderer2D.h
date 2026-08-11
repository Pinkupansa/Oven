#pragma once 
#include "Oven/Renderer/OrthographicCamera.h"
#include "Oven/Renderer/Texture.h"
#include "Oven/Renderer/SubTexture.h"
namespace Oven{
    
    class Renderer2D{
        public: 
            static void Init();
            static void Shutdown();                           
            static void BeginScene(const OrthographicCamera& camera);
            static void EndScene(); 
            static void Flush(); 

            static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f});
            static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f});
            static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2 tilingFactor = {1.0f, 1.0f});
            static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2 tilingFactor = {1.0f, 1.0f});
            static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2 tilingFactor = {1.0f, 1.0f});
            static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2 tilingFactor = {1.0f, 1.0f});
            
            static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f});
            static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f});
            static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2 tilingFactor = {1.0f, 1.0f});
            static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2 tilingFactor = {1.0f, 1.0f});
            static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2 tilingFactor = {1.0f, 1.0f});
            static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2 tilingFactor = {1.0f, 1.0f});
           
            
            struct Statistics
            {
                uint32_t DrawCalls = 0;
                uint32_t QuadCount = 0; 
                
                uint32_t GetTotalVertexCount() { return QuadCount * 4; }
                uint32_t GetTotalIndexCount() { return QuadCount * 6; }
            };
            static Statistics GetStats();
            static void ResetStats();

        private:
            static void EndAndReset();
            static void PushQuad(const glm::vec4* positions, const glm::vec4& color, const glm::vec2* uvs, float texIndex, const glm::vec2 tilingFactor);
    };
}