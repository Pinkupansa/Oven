#include "UI/Panels/EditorPanel.h"
namespace Oven
{
class Toolbar : public EditorPanel
{
public:
    void OnImGuiRender();
    void OnAttach();

private:
    Ref<Texture2D> m_IconPlay, m_IconStop;
};
}; // namespace Oven