#include "Oven/ovenpch.h"
#include "Oven/Renderer/SubTexture.h"

namespace Oven
{
Oven::SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max)
    : m_Texture(texture)
{
    m_UVs[0] = {min.x, min.y};
    m_UVs[1] = {max.x, min.y};
    m_UVs[2] = {max.x, max.y};
    m_UVs[3] = {min.x, max.y};
}

Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D>& texture,
                                                 const glm::vec2& coords,
                                                 const glm::vec2& sheetCellSize,
                                                 const glm::vec2& spriteSizeInCells,
                                                 uint32_t sheetPadding)
{
    static const float inShift = 0.01;
    glm::vec2 min = {((coords.x * (sheetCellSize.x + sheetPadding) + inShift) / texture->GetWidth()),
                     ((coords.y * (sheetCellSize.y + sheetPadding) + inShift) / texture->GetHeight())};
    glm::vec2 max = min + glm::vec2{(spriteSizeInCells.x * sheetCellSize.x - 2 * inShift) / texture->GetWidth(),
                                    (spriteSizeInCells.y * sheetCellSize.y - 2 * inShift) / texture->GetHeight()};

    return CreateRef<SubTexture2D>(texture, min, max);
}
} // namespace Oven
