#pragma once 
#include "Oven/Renderer/OrthographicCamera.h"
#include "Oven/Renderer/Texture.h"
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
            
            static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f});
            static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f});
            static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2 tilingFactor = {1.0f, 1.0f});
            static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2 tilingFactor = {1.0f, 1.0f});

    };
}