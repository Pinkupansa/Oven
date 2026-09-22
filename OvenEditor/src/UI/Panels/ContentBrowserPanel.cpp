#include "UI/Panels/ContentBrowserPanel.h"
namespace Oven
{

static std::filesystem::path s_AssetsPath = "OvenEditor/assets";

ContentBrowserPanel::ContentBrowserPanel() : m_CurrentDirectory(s_AssetsPath) {}
void ContentBrowserPanel::OnImGuiRender()
{
    ImGui::Begin("Browser");
    if (m_CurrentDirectory != s_AssetsPath)
    {
        if (ImGui::Button("<-"))
        {
            m_CurrentDirectory = m_CurrentDirectory.parent_path();
        }
    }

    static float padding = 16.0f;
    static float thumbnaiSize = 64.0f;
    float cellSize = thumbnaiSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1)
        columnCount = 1;

    ImGui::Columns(columnCount, 0, false);
    for (auto& p : std::filesystem::directory_iterator(m_CurrentDirectory))
    {

        auto relativePath = std::filesystem::relative(p, s_AssetsPath);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        ImGui::ImageButton(
            p.path().filename().c_str(),
            (ImTextureID)(p.is_directory() ? m_IconTheme.FolderIcon->GetRendererID() : GetFileIcon(p)->GetRendererID()),
            {thumbnaiSize, thumbnaiSize},
            {0, 1},
            {1, 0}
        );
        if (ImGui::BeginDragDropSource())
        {
            // 1. Get the native path pointer directly from std::filesystem::path
            const auto* itemPath = p.path().c_str();

            // 2. Calculate full byte size including the null terminator
            size_t sizeInBytes = (p.path().native().size() + 1) * sizeof(std::filesystem::path::value_type);

            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, sizeInBytes, ImGuiCond_Once);
            ImGui::EndDragDropSource();
        }
        ImGui::PopStyleColor(2);

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {}
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (p.is_directory())
            {
                m_CurrentDirectory /= p.path().filename();
            }
            else
            {
            }
        }
        ImGui::TextWrapped("%s", p.path().filename().c_str());
        ImGui::NextColumn();
    }
    ImGui::Columns(1);
    ImGui::End();
}
void ContentBrowserPanel::OnEvent(Event& e) {}

void ContentBrowserPanel::OnAttach()
{
    m_IconTheme.FolderIcon = Texture2D::Create("OvenEditor/resources/icons/folder.png");
    m_IconTheme.ImageIcon = Texture2D::Create("OvenEditor/resources/icons/image.png");
    m_IconTheme.PrefabIcon = Texture2D::Create("OvenEditor/resources/icons/object.png");
    m_IconTheme.ScriptIcon = Texture2D::Create("OvenEditor/resources/icons/cpp.png");
    m_IconTheme.DefaultIcon = Texture2D::Create("OvenEditor/resources/icons/default.png");
    m_IconTheme.SceneIcon = Texture2D::Create("OvenEditor/resources/icons/scene.png");
}

Ref<Texture2D> ContentBrowserPanel::GetFileIcon(std::filesystem::path path)
{
    std::string extension = path.extension().string();
    if (extension == ".png")
    {
        return m_IconTheme.ImageIcon;
    }
    if (extension == ".prefab")
    {
        return m_IconTheme.PrefabIcon;
    }
    if (extension == ".cpp" || extension == ".cs" || extension == ".h" || extension == ".hpp")
    {
        return m_IconTheme.ScriptIcon;
    }
    if (extension == ".oven")
    {
        return m_IconTheme.SceneIcon;
    }
    return m_IconTheme.DefaultIcon;
}

} // namespace Oven