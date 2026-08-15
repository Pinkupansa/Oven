#include <iostream>
#include <Oven.h>
#include <glm/vec3.hpp>                  // glm::vec3
#include <glm/vec4.hpp>                  // glm::vec4
#include <glm/mat4x4.hpp>                // glm::mat4
#include <glm/ext/matrix_transform.hpp>  // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp>  // glm::pi
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "EditorLayer.h"
#include <Oven/Core/EntryPoint.h>

namespace Oven
{
class OvenEditor : public Application
{
public:
    OvenEditor() : Application("Oven Editor")
    {
        // PushLayer(new TestLayer());
        PushLayer(new EditorLayer());
    }
    ~OvenEditor() {}
};

Application* CreateApplication()
{
    printf("Launching Oven Editor ...\n");
    return new OvenEditor();
}
} // namespace Oven