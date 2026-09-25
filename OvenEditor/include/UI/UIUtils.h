#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <string>
#include <algorithm>
#include <typeinfo>
#include <utility>
#include "UI/EditorColors.h"

namespace Oven
{

class UIUtils
{
public:
    constexpr static float DEFAULT_ROUNDING = 3.0f;
    constexpr static float SHADOW_HEIGHT_RATIO = 0.08f;
    constexpr static float PRESS_Y_OFFSET = 1.5f;
    constexpr static float BEVEL_THICKNESS = 1.0f;
    constexpr static float BORDER_THICKNESS = 1.0f;

    // Burger Icon Ratios relative to button frame height
    constexpr static float BURGER_WIDTH_RATIO = 0.45f;
    constexpr static float BURGER_BAR_HEIGHT_RATIO = 0.07f;
    constexpr static float BURGER_BAR_GAP_RATIO = 0.10f;
    constexpr static float BURGER_BAR_ROUNDING = 0.5f;

    // Property rows layout
    constexpr static float PROPERTY_RIGHT_MARGIN = 30.0f;
    constexpr static float PROPERTY_ITEM_SPACING = 4.0f;

    // ======================================================================
    // Couleurs / dessin bas niveau
    // ======================================================================

    // Convert palette ImVec4 to ImU32 with optional alpha override
    static ImU32 ToImU32(const ImVec4& color, float alphaOverride = -1.0f)
    {
        ImVec4 c = color;
        if (alphaOverride >= 0.0f)
            c.w = alphaOverride;
        return ImGui::ColorConvertFloat4ToU32(c);
    }

    // Applies a vertical gradient across tessellated rounded vertices
    static void AddRectFilledGradientRounded(
        ImDrawList* drawList, const ImVec2& p_min, const ImVec2& p_max, ImU32 col_top, ImU32 col_bot, float rounding
    )
    {
        const int vtxStart = drawList->VtxBuffer.Size;
        drawList->AddRectFilled(p_min, p_max, col_top, rounding);
        const int vtxEnd = drawList->VtxBuffer.Size;

        const float height = p_max.y - p_min.y;
        if (height <= 0.0f)
            return;

        const ImVec4 top = ImGui::ColorConvertU32ToFloat4(col_top);
        const ImVec4 bot = ImGui::ColorConvertU32ToFloat4(col_bot);

        for (int i = vtxStart; i < vtxEnd; ++i)
        {
            ImDrawVert& v = drawList->VtxBuffer[i];
            const float t = ImClamp((v.pos.y - p_min.y) / height, 0.0f, 1.0f);
            v.col = ImGui::ColorConvertFloat4ToU32(ImLerp(top, bot, t));
        }
    }

    static ImVec4 Shade(const ImVec4& c, float amount)
    { return ImVec4(ImSaturate(c.x + amount), ImSaturate(c.y + amount), ImSaturate(c.z + amount), c.w); }

    // Trait horizontal de 1 px à l'intérieur du cadre, utilisé pour le biseau.
    static void AddInnerEdge(ImDrawList* drawList, float x0, float x1, float y, ImU32 col, float rounding)
    { drawList->AddLine(ImVec2(x0 + rounding, y), ImVec2(x1 - rounding, y), col, BEVEL_THICKNESS); }

    // ======================================================================
    // Boutons tactiles
    // ======================================================================

    // Socle commun : zone cliquable + cadre, puis délègue le dessin du contenu.
    // renderContent(ImDrawList*, ImVec2 center, ImVec2 size, ImU32 contentColor)
    template <typename RenderContentFunc>
    static bool
    TactileButtonCore(const char* str_id, const ImVec2& size_arg, float rounding, RenderContentFunc&& renderContent)
    {
        const float frameHeight = ImGui::GetFrameHeight();
        const ImVec2 size = ResolveSize(size_arg, ImVec2(frameHeight, frameHeight));

        // Centre verticalement les boutons plus petits qu'une ligne standard
        const float offsetY = (frameHeight - size.y) * 0.5f;
        if (offsetY > 0.0f)
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);

        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const bool pressed = ImGui::InvisibleButton(str_id, size);
        const bool hovered = ImGui::IsItemHovered();

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        DrawTactileFrame(drawList, pos, size, rounding, hovered);

        const ImVec2 center(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);
        renderContent(drawList, center, size, ToImU32(COLOR_TEXT_DEFAULT));

        return pressed;
    }

    // Text Gradient Button
    static bool
    TactileGradientButton(const char* label, const ImVec2& size_arg = ImVec2(0, 0), float rounding = DEFAULT_ROUNDING)
    {
        const ImGuiStyle& style = ImGui::GetStyle();
        const char* labelEnd = ImGui::FindRenderedTextEnd(label); // ignore la partie "##id"
        const ImVec2 textSize = ImGui::CalcTextSize(label, labelEnd);
        const ImVec2 size =
            ResolveSize(size_arg, ImVec2(textSize.x + style.FramePadding.x * 2.0f, ImGui::GetFrameHeight()));

        return TactileButtonCore(label, size, rounding, [&](ImDrawList* drawList, ImVec2 center, ImVec2, ImU32 color) {
            const ImVec2 textPos(center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f);
            drawList->AddText(textPos, color, label, labelEnd);
        });
    }

    // Burger Icon Button
    static bool
    TactileBurgerButton(const char* str_id, const ImVec2& size_arg = ImVec2(0, 0), float rounding = DEFAULT_ROUNDING)
    {
        return TactileButtonCore(
            str_id, size_arg, rounding, [](ImDrawList* drawList, ImVec2 center, ImVec2 size, ImU32 color) {
                const float minDim = ImMin(size.x, size.y);
                const float barWidth = minDim * BURGER_WIDTH_RATIO;
                const float barHeight = ImMax(1.0f, minDim * BURGER_BAR_HEIGHT_RATIO);
                const float barGap = minDim * BURGER_BAR_GAP_RATIO;
                const float totalHeight = (3.0f * barHeight) + (2.0f * barGap);

                const float leftX = center.x - (barWidth * 0.5f);
                const float rightX = center.x + (barWidth * 0.5f);
                const float startY = center.y - (totalHeight * 0.5f);

                for (int i = 0; i < 3; ++i)
                {
                    const float y = startY + i * (barHeight + barGap);
                    drawList->AddRectFilled(
                        ImVec2(leftX, y), ImVec2(rightX, y + barHeight), color, BURGER_BAR_ROUNDING
                    );
                }
            }
        );
    }

    // Image Button
    // - imageSize : taille de l'image affichée
    // - size_arg  : taille totale du bouton (0 = imageSize + FramePadding * 2, comme ImGui::ImageButton)
    // - uv0 / uv1 : passer (0,1) / (1,0) pour une texture OpenGL chargée à l'envers
    // - tint      : blanc = couleurs d'origine de l'image
    static bool TactileImageButton(
        const char* str_id,
        ImTextureID textureId,
        const ImVec2& imageSize,
        const ImVec2& size_arg = ImVec2(0, 0),
        float rounding = DEFAULT_ROUNDING,
        const ImVec2& uv0 = ImVec2(0, 0),
        const ImVec2& uv1 = ImVec2(1, 1),
        const ImVec4& tint = ImVec4(1, 1, 1, 1)
    )
    {
        const ImVec2 padding = ImGui::GetStyle().FramePadding;
        const ImVec2 size =
            ResolveSize(size_arg, ImVec2(imageSize.x + padding.x * 2.0f, imageSize.y + padding.y * 2.0f));

        return TactileButtonCore(str_id, size, rounding, [&](ImDrawList* drawList, ImVec2 center, ImVec2, ImU32) {
            // Arrondi au pixel pour éviter une image floue
            const ImVec2 imgMin = ImFloor(ImVec2(center.x - imageSize.x * 0.5f, center.y - imageSize.y * 0.5f));
            const ImVec2 imgMax(imgMin.x + imageSize.x, imgMin.y + imageSize.y);
            drawList->AddImage(textureId, imgMin, imgMax, uv0, uv1, ToImU32(tint));
        });
    }

    // ======================================================================
    // Lignes de propriétés (label | contrôle)
    // ======================================================================

    // Helper to standardize the 2-column property table layout
    static bool BeginPropertyRow(const char* label, float columnWidth)
    {
        ImGui::PushID(label);

        if (!ImGui::BeginTable("##PropertyTable", 2, ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::PopID(); // garde la pile d'ID équilibrée si la table n'est pas soumise
            return false;
        }

        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
        ImGui::TableSetupColumn("Controls", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextRow();

        // Column 0: Text Label
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);

        // Column 1: Controls
        ImGui::TableSetColumnIndex(1);
        return true;
    }

    static void EndPropertyRow()
    {
        ImGui::EndTable();
        ImGui::PopID();
    }

    // Enveloppe Begin/EndPropertyRow. drawControl() doit renvoyer true si la valeur a changé.
    template <typename DrawControlFunc>
    static bool PropertyRow(const char* label, float columnWidth, DrawControlFunc&& drawControl)
    {
        if (!BeginPropertyRow(label, columnWidth))
            return false;

        const bool changed = drawControl();
        EndPropertyRow();
        return changed;
    }

    // String Buffer Overload
    static bool InputField(const char* label, char* buffer, float columnWidth = 100.0f)
    {
        return PropertyRow(label, columnWidth, [&] {
            ScopedStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(PROPERTY_ITEM_SPACING, 0.0f));
            SetPropertyItemWidth();
            // ATTENTION : sizeof(buffer) vaut la taille d'un pointeur (8), pas celle du tableau.
            return ImGui::InputText("##value", buffer, sizeof(buffer));
        });
    }

    // Single Float Drag Control
    static bool DrawFloatControl(
        const char* label,
        float& value,
        float resetValue = 0.0f,
        float columnWidth = 100.0f,
        float speed = 0.1f,
        float min = 0.0f,
        float max = 0.0f,
        const char* format = "%.2f",
        bool variableSpeed = true
    )
    {
        (void)variableSpeed; // pas encore utilisé

        return PropertyRow(label, columnWidth, [&] {
            ScopedStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(PROPERTY_ITEM_SPACING, 0.0f));
            SetPropertyItemWidth();

            bool changed = ImGui::DragFloat("##value", &value, speed, min, max, format);
            changed |= ResetOnRightClick(value, resetValue);
            return changed;
        });
    }

    struct AxisInfo
    {
        const char* label;
        const char* id;
        ImVec4 color;
    };

    static constexpr AxisInfo g_AxisConfigs[] = {
        {"X", "##X", ImVec4{0.85f, 0.25f, 0.25f, 1.0f}},
        {"Y", "##Y", ImVec4{0.25f, 0.75f, 0.25f, 1.0f}},
        {"Z", "##Z", ImVec4{0.25f, 0.45f, 0.90f, 1.0f}},
        {"W", "##W", ImVec4{0.85f, 0.65f, 0.25f, 1.0f}}
    };

    // Generic vector control renderer
    static bool DrawVecControlImpl(
        const char* label,
        float* values,
        int count,
        float resetValue = 0.0f,
        float columnWidth = 100.0f,
        float speed = 0.1f,
        const char* format = "%.2f"
    )
    {
        IM_ASSERT(count >= 1 && count <= IM_ARRAYSIZE(g_AxisConfigs));

        return PropertyRow(label, columnWidth, [&] {
            constexpr float groupSpacing = 8.0f; // Spacing between component groups

            ScopedStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(PROPERTY_ITEM_SPACING, 0.0f));
            ImFont* boldFont = GetBoldFont();

            // Largeur des labels, mesurée avec la police utilisée pour les dessiner
            float totalLabelWidth = 0.0f;
            {
                ScopedFont font(boldFont);
                for (int i = 0; i < count; ++i)
                    totalLabelWidth += ImGui::CalcTextSize(g_AxisConfigs[i].label).x;
            }

            // Distribute remaining width equally among inputs
            const float nonInputWidth =
                totalLabelWidth + (count * PROPERTY_ITEM_SPACING) + ((count - 1) * groupSpacing);
            const float inputWidth = ImMax(1.0f, (GetPropertyAvailWidth() - nonInputWidth) / static_cast<float>(count));

            bool changed = false;
            for (int i = 0; i < count; ++i)
            {
                const AxisInfo& axis = g_AxisConfigs[i];

                // Axis label
                {
                    ScopedStyleColor textColor(ImGuiCol_Text, axis.color);
                    ScopedFont font(boldFont);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted(axis.label);
                }

                ImGui::SameLine();

                // Axis value
                ImGui::SetNextItemWidth(inputWidth);
                changed |= ImGui::DragFloat(axis.id, &values[i], speed, 0.0f, 0.0f, format);
                changed |= ResetOnRightClick(values[i], resetValue);

                if (i < count - 1)
                    ImGui::SameLine(0.0f, groupSpacing);
            }
            return changed;
        });
    }

    // Vector2 Control
    static bool DrawVec2Control(
        const char* label,
        glm::vec2& values,
        float resetValue = 0.0f,
        float columnWidth = 100.0f,
        float speed = 0.1f,
        const char* format = "%.2f"
    )
    { return DrawVecControlImpl(label, &values.x, 2, resetValue, columnWidth, speed, format); }

    // Vector3 Control
    static bool DrawVec3Control(
        const char* label,
        glm::vec3& values,
        float resetValue = 0.0f,
        float columnWidth = 100.0f,
        float speed = 0.1f,
        const char* format = "%.2f"
    )
    { return DrawVecControlImpl(label, &values.x, 3, resetValue, columnWidth, speed, format); }

    // Vector4 Control
    static bool DrawVec4Control(
        const char* label,
        glm::vec4& values,
        float resetValue = 0.0f,
        float columnWidth = 100.0f,
        float speed = 0.1f,
        const char* format = "%.2f"
    )
    { return DrawVecControlImpl(label, &values.x, 4, resetValue, columnWidth, speed, format); }

    // Combo Box / Dropdown Control with Left-Aligned Label
    template <typename EnumType>
    static bool DrawComboControl(
        const char* label,
        EnumType& currentEnumValue,
        const char* const optionStrings[],
        int optionCount,
        float columnWidth = 100.0f
    )
    {
        return PropertyRow(label, columnWidth, [&] {
            const int currentIndex = static_cast<int>(currentEnumValue);
            const char* currentLabel =
                (currentIndex >= 0 && currentIndex < optionCount) ? optionStrings[currentIndex] : "Unknown";

            SetPropertyItemWidth();

            bool changed = false;
            if (ImGui::BeginCombo("##value", currentLabel))
            {
                for (int i = 0; i < optionCount; i++)
                {
                    const bool isSelected = (currentIndex == i);
                    if (ImGui::Selectable(optionStrings[i], isSelected))
                    {
                        currentEnumValue = static_cast<EnumType>(i);
                        changed = true;
                    }

                    // Set initial focus to current selection when popup opens
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            return changed;
        });
    }

    // ======================================================================
    // Divers
    // ======================================================================

    static void Checkbox(const std::string& name, bool& target, float size = 0.5f)
    {
        ScopedStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(size, size));
        ImGui::Checkbox(name.c_str(), &target);
    }

    template <typename T, typename UIFunction>
    static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
    {
        if (!entity.HasComponent<T>())
            return;

        ImGui::PushID(name.c_str());

        auto& component = entity.GetComponent<T>();
        const ImGuiTreeNodeFlags treeNodeFlags =
            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;
        const ImVec2 buttonSize(20.0f, 20.0f);

        bool open = false;
        {
            ScopedStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
            ImGui::Separator();
            {
                ScopedFont font(GetBoldFont());
                // Correct template type hashing for unique tree nodes
                open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name.c_str());
            }
            // Align button to right edge
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - buttonSize.x);
        }

        if (TactileBurgerButton("##ComponentSettingsButton", buttonSize))
            ImGui::OpenPopup("ComponentSettings");

        bool removeComponent = false;
        if (ImGui::BeginPopup("ComponentSettings"))
        {
            if (ImGui::MenuItem("Remove Component"))
                removeComponent = true;
            ImGui::EndPopup();
        }

        if (open)
        {
            uiFunction(component);
            ImGui::TreePop();
        }

        if (removeComponent)
            entity.RemoveComponent<T>();

        ImGui::PopID();
    }

    static bool MenuItem(const char* label)
    {
        ScopedStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
        ScopedStyleVar align(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
        return ImGui::MenuItem(label);
    }

    static void CenterElement(float elementWidth)
    {
        const float offsetX = (ImGui::GetContentRegionAvail().x - elementWidth) * 0.5f;
        if (offsetX > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
    }

    static void PanelContentSeparator() { ImGui::Dummy(ImVec2(0.0f, 1.0f)); }

private:
    // ======================================================================
    // RAII : garantit un Pop pour chaque Push, même en cas de return anticipé
    // ======================================================================
    struct ScopedStyleVar
    {
        ScopedStyleVar(ImGuiStyleVar idx, const ImVec2& value) { ImGui::PushStyleVar(idx, value); }
        ~ScopedStyleVar() { ImGui::PopStyleVar(); }
        ScopedStyleVar(const ScopedStyleVar&) = delete;
        ScopedStyleVar& operator=(const ScopedStyleVar&) = delete;
    };

    struct ScopedStyleColor
    {
        ScopedStyleColor(ImGuiCol idx, const ImVec4& color) { ImGui::PushStyleColor(idx, color); }
        ~ScopedStyleColor() { ImGui::PopStyleColor(); }
        ScopedStyleColor(const ScopedStyleColor&) = delete;
        ScopedStyleColor& operator=(const ScopedStyleColor&) = delete;
    };

    struct ScopedFont
    {
        explicit ScopedFont(ImFont* font) : m_Pushed(font != nullptr)
        {
            if (m_Pushed)
                ImGui::PushFont(font);
        }
        ~ScopedFont()
        {
            if (m_Pushed)
                ImGui::PopFont();
        }
        ScopedFont(const ScopedFont&) = delete;
        ScopedFont& operator=(const ScopedFont&) = delete;

    private:
        bool m_Pushed;
    };

    // ======================================================================
    // Helpers internes
    // ======================================================================

    // Remplace les composantes <= 0 par celles de fallback
    static ImVec2 ResolveSize(const ImVec2& size, const ImVec2& fallback)
    { return ImVec2(size.x > 0.0f ? size.x : fallback.x, size.y > 0.0f ? size.y : fallback.y); }

    static ImFont* GetBoldFont()
    {
        ImGuiIO& io = ImGui::GetIO();
        return io.Fonts->Fonts.Size > 0 ? io.Fonts->Fonts[0] : nullptr;
    }

    static float GetPropertyAvailWidth() { return ImGui::GetContentRegionAvail().x - PROPERTY_RIGHT_MARGIN; }

    static void SetPropertyItemWidth() { ImGui::SetNextItemWidth(GetPropertyAvailWidth()); }

    // Clic droit sur le dernier item : remet la valeur par défaut
    static bool ResetOnRightClick(float& value, float resetValue)
    {
        if (!ImGui::IsItemClicked(ImGuiMouseButton_Right))
            return false;
        value = resetValue;
        return true;
    }

    // Cadre commun à tous les boutons tactiles : fond dégradé, ombre, biseau, contour
    static void
    DrawTactileFrame(ImDrawList* drawList, const ImVec2& pos, const ImVec2& size, float rounding, bool hovered)
    {
        const ImVec2 maxPos(pos.x + size.x, pos.y + size.y);
        const ImVec4 base = COLOR_BUTTON_DEFAULT;

        // Le survol reste le seul changement d'état visible
        const float lift = hovered ? 0.045f : 0.0f;
        const ImVec4 borderCol = hovered ? COLOR_HOVERED_DEFAULT : COLOR_BORDER_DEFAULT;

        // Amplitude du dégradé conservée dans tous les états, sinon le bouton s'aplatit au survol
        constexpr float GRADIENT_SPREAD = 0.045f;
        const ImU32 colTop = ToImU32(Shade(base, lift + GRADIENT_SPREAD));
        const ImU32 colBottom = ToImU32(Shade(base, lift - GRADIENT_SPREAD));

        // 1. Fond, rentré d'un demi-contour pour rester à l'intérieur du trait
        const float halfBorder = BORDER_THICKNESS * 0.5f;
        const ImVec2 bgMin(pos.x + halfBorder, pos.y + halfBorder);
        const ImVec2 bgMax(maxPos.x - halfBorder, maxPos.y - halfBorder);
        const float bgRounding = ImMax(0.0f, rounding - halfBorder);
        AddRectFilledGradientRounded(drawList, bgMin, bgMax, colTop, colBottom, bgRounding);

        // 2. Ombre portée interne basse
        const float shadowH = ImMax(1.0f, size.y * SHADOW_HEIGHT_RATIO);
        drawList->AddRectFilled(
            ImVec2(bgMin.x, bgMax.y - shadowH),
            bgMax,
            ToImU32(ImVec4(0.0f, 0.0f, 0.0f, 0.28f)),
            bgRounding,
            ImDrawFlags_RoundCornersBottom
        );

        // 3. Biseau clair sur l'arête haute
        AddInnerEdge(
            drawList, bgMin.x, bgMax.x, bgMin.y + BEVEL_THICKNESS * 0.5f, ToImU32(Shade(base, 0.16f)), bgRounding
        );

        // 4. Contour extérieur
        drawList->AddRect(pos, maxPos, ToImU32(borderCol), rounding, 0, BORDER_THICKNESS);
    }
};

} // namespace Oven