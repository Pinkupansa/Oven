#pragma once 
#include "Oven/Renderer/Texture.h"
#include <glm/glm.hpp>
namespace Oven{

    class SubTexture2D{
        public:
            SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);
            const glm::vec2* GetUVs() const { return m_UVs; }
            const Ref<Texture2D> GetTexture() const { return m_Texture; }

        static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& sheetCellSize, glm::vec2 spriteSizeCells = {1, 1});
        private:
            Ref<Texture2D> m_Texture;
            glm::vec2 m_UVs[4];
    };
}   