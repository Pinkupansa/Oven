#pragma once
#include "Oven/ovenpch.h"
#include "Oven/Renderer/Texture.h"
#include "UI/Panels/EditorPanel.h"
#include <filesystem>
namespace Oven
{
class ContentBrowserPanel : public EditorPanel
{
public:
    ContentBrowserPanel();
    void OnImGuiRender() override;
    void OnEvent(Event& e) override;
    void OnAttach() override;

private:
    std::filesystem::path m_CurrentDirectory;
    struct IconTheme
    {
        Ref<Texture2D> FolderIcon;
        Ref<Texture2D> ScriptIcon;
        Ref<Texture2D> ImageIcon;
        Ref<Texture2D> PrefabIcon;
        Ref<Texture2D> SceneIcon;
        Ref<Texture2D> DefaultIcon;
    };

    Ref<Texture2D> GetFileIcon(std::filesystem::path path);
    IconTheme m_IconTheme;
};
} // namespace Oven