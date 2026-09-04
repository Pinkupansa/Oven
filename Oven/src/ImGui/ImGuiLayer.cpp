#include "Oven/ImGui/ImGuiLayer.h"
#include "imgui.h"
#include "Oven/Core/Application.h"
#include "Oven/Core/Core.h"
#include "Oven/Core/Log.h"
#include "Oven/ImGui/ImGuiLayer.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

// TEMPORARY
#include "GLFW/glfw3.h"
#include "ImGuizmo.h"

namespace Oven
{

/// Temporary function until we have a proper key mapping system
ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::OnAttach()
{
    OVEN_PROFILE_FUNCTION();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    //   io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    //  io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //  io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
    // Setup Dear ImGui style

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    Application& app = Application::Get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::OnImGuiRender() {}

void ImGuiLayer::OnDetach()
{
    OVEN_PROFILE_FUNCTION();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::OnEvent(Event& e)
{
    if (m_BlockEvents)
    {
        EventDispatcher dispatcher(e);
        dispatcher.DispatchCategory(OVEN_BIND_EVENT_FN(ImGuiLayer::OnMouseEvent), EventCategory::EventCategoryMouse);
        dispatcher.DispatchCategory(
            OVEN_BIND_EVENT_FN(ImGuiLayer::OnKeyboardEvent), EventCategory::EventCategoryKeyboard
        );
    }
}

void ImGuiLayer::ResizeWindow(uint32_t width, uint32_t height)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(width, height);
    glViewport(0, 0, width, height);
}

bool ImGuiLayer::OnMouseEvent(Event& e)
{
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

bool ImGuiLayer::OnKeyboardEvent(Event& e)
{
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureKeyboard;
}

void ImGuiLayer::RescaleContent(float scaleX, float scaleY)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplayFramebufferScale = ImVec2(scaleX, scaleY);
}

void ImGuiLayer::Begin()
{
    OVEN_PROFILE_FUNCTION();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void ImGuiLayer::End()
{
    OVEN_PROFILE_FUNCTION();

    ImGuiIO& io = ImGui::GetIO();
    Application& app = Application::Get();
    io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

} // namespace Oven