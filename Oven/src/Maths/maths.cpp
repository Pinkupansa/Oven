#include "Oven/ovenpch.h"
#include "Oven/Maths/maths.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/epsilon.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
namespace Oven::Maths
{
bool DecomposeTransform(
    const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale
)
{
    glm::mat4 LocalMatrix(transform);

    // 1. Perspective check
    if (glm::epsilonEqual(LocalMatrix[3][3], 0.0f, glm::epsilon<float>()))
        return false;

    // 2. Extract Translation
    outTranslation = glm::vec3(LocalMatrix[3]);
    LocalMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // 3. Extract Scale and Orthonormalize basis vectors
    glm::vec3 Row[3];
    for (int i = 0; i < 3; ++i)
        Row[i] = glm::vec3(LocalMatrix[i]); // Row[i] holds Column i in GLM column-major layout

    outScale.x = glm::length(Row[0]);
    Row[0] = glm::normalize(Row[0]);

    outScale.y = glm::length(Row[1]);
    Row[1] = glm::normalize(Row[1]);

    outScale.z = glm::length(Row[2]);
    Row[2] = glm::normalize(Row[2]);

    // 4. Handle Negative Scale / Reflection
    glm::vec3 crossProduct = glm::cross(Row[1], Row[2]);
    if (glm::dot(Row[0], crossProduct) < 0.0f)
    {
        outScale *= -1.0f;
        Row[0] *= -1.0f;
        Row[1] *= -1.0f;
        Row[2] *= -1.0f;
    }

    // 5. Extract Euler Angles matching R_y * R_x * R_z construction
    // Row[col][row] in GLM indexing:
    float sinX = glm::clamp(-Row[2][1], -1.0f, 1.0f);
    outRotation.x = glm::asin(sinX);

    if (glm::abs(glm::cos(outRotation.x)) > 0.0001f) // Non-singular case
    {
        outRotation.y = glm::atan(Row[2][0], Row[2][2]);
        outRotation.z = glm::atan(Row[0][1], Row[1][1]);
    }
    else // Gimbal Lock fallback (Pitch is at +/- 90 degrees)
    {
        outRotation.y = glm::atan(Row[1][0], Row[0][0]);
        outRotation.z = 0.0f;
    }

    return true;
}
} // namespace Oven::Maths
