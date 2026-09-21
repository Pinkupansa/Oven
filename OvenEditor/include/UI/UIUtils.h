#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <string>
#include <algorithm>
#include <typeinfo>
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
            const ImVec4 c = ImVec4(
                top.x + t * (bot.x - top.x),
                top.y + t * (bot.y - top.y),
                top.z + t * (bot.z - top.z),
                top.w + t * (bot.w - top.w)
            );
            v.col = ImGui::ColorConvertFloat4ToU32(c);
        }
    }

    static ImVec4 Shade(const ImVec4& c, float amount)
    { return ImVec4(ImSaturate(c.x + amount), ImSaturate(c.y + amount), ImSaturate(c.z + amount), c.w); }

    // Trait horizontal de 1 px à l'intérieur du cadre, utilisé pour le biseau.
    static void AddInnerEdge(ImDrawList* drawList, float x0, float x1, float y, ImU32 col, float rounding)
    { drawList->AddLine(ImVec2(x0 + rounding, y), ImVec2(x1 - rounding, y), col, BEVEL_THICKNESS); }

    template <typename RenderContentFunc>
    static bool
    TactileButtonCore(const char* str_id, const ImVec2& size_arg, float rounding, RenderContentFunc&& renderContent)
    {
        const float frameHeight = ImGui::GetFrameHeight();

        ImVec2 size = size_arg;
        if (size.x <= 0.0f)
            size.x = frameHeight;
        if (size.y <= 0.0f)
            size.y = frameHeight;

        const float offsetY = (frameHeight - size.y) * 0.5f;
        if (offsetY > 0.0f)
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);

        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const bool pressed = ImGui::InvisibleButton(str_id, size);
        const bool hovered = ImGui::IsItemHovered();
        const bool held = ImGui::IsItemActive();

        // IsItemActive reste vrai si l'on maintient le clic en sortant du bouton,
        // alors que le relâchement ne déclenchera rien.
        const bool sunken = held && hovered;

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const ImVec2 maxPos = ImVec2(pos.x + size.x, pos.y + size.y);

        // ------------------------------------------------------------------
        // Couleurs d'état. Le survol reste le seul changement franc ; le clic
        // se contente de retirer un peu de luminosité.
        // ------------------------------------------------------------------
        const ImVec4 base = COLOR_BUTTON_DEFAULT;

        float lift = 0.0f;
        ImVec4 borderCol = COLOR_BORDER_DEFAULT;

        if (hovered)
        {
            lift = 0.045f;
            borderCol = COLOR_HOVERED_DEFAULT;
        }

        // Amplitude du dégradé conservée dans tous les états, sinon le bouton
        // s'aplatit au survol.
        constexpr float GRADIENT_SPREAD = 0.045f;
        const ImU32 colTop = ToImU32(Shade(base, lift + GRADIENT_SPREAD));
        const ImU32 colBottom = ToImU32(Shade(base, lift - GRADIENT_SPREAD));
        const ImU32 colContent = ToImU32(COLOR_TEXT_DEFAULT); // Ne change pas d'état en état
        const ImU32 colBorder = ToImU32(borderCol);

        // ------------------------------------------------------------------
        // 1. Fond, rentré d'un demi-contour pour rester à l'intérieur du trait
        // ------------------------------------------------------------------
        const float halfBorder = BORDER_THICKNESS * 0.5f;
        const ImVec2 bgMin = ImVec2(pos.x + halfBorder, pos.y + halfBorder);
        const ImVec2 bgMax = ImVec2(maxPos.x - halfBorder, maxPos.y - halfBorder);
        const float bgRounding = ImMax(0.0f, rounding - halfBorder);

        AddRectFilledGradientRounded(drawList, bgMin, bgMax, colTop, colBottom, bgRounding);

        // ------------------------------------------------------------------
        // 2. Ombre portée interne basse, identique dans tous les états
        // ------------------------------------------------------------------
        const float shadowH = ImMax(1.0f, size.y * SHADOW_HEIGHT_RATIO);
        const ImU32 shadowCol = ToImU32(ImVec4(0.0f, 0.0f, 0.0f, 0.28f));
        drawList->AddRectFilled(
            ImVec2(bgMin.x, bgMax.y - shadowH), bgMax, shadowCol, bgRounding, ImDrawFlags_RoundCornersBottom
        );

        // ------------------------------------------------------------------
        // 3. Biseau clair sur l'arête haute, identique dans tous les états
        // ------------------------------------------------------------------
        const ImU32 bevelCol = ToImU32(Shade(base, 0.16f));
        AddInnerEdge(drawList, bgMin.x, bgMax.x, bgMin.y + BEVEL_THICKNESS * 0.5f, bevelCol, bgRounding);

        // ------------------------------------------------------------------
        // 4. Contour extérieur
        // ------------------------------------------------------------------
        drawList->AddRect(pos, maxPos, colBorder, rounding, 0, BORDER_THICKNESS);

        // ------------------------------------------------------------------
        // 5. Contenu. PRESS_Y_OFFSET à 0 laisse le contenu parfaitement fixe.
        // ------------------------------------------------------------------
        ImVec2 contentCenter = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

        renderContent(drawList, contentCenter, size, colContent);

        return pressed;
    }
    // Text Gradient Button
    static bool
    TactileGradientButton(const char* label, const ImVec2& size_arg = ImVec2(0, 0), float rounding = DEFAULT_ROUNDING)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        const ImVec2 textSize = ImGui::CalcTextSize(label, nullptr, true);

        ImVec2 size = size_arg;
        if (size.x <= 0.0f)
            size.x = textSize.x + style.FramePadding.x * 2.0f;
        if (size.y <= 0.0f)
            size.y = ImGui::GetFrameHeight();

        return TactileButtonCore(
            label, size, rounding, [&](ImDrawList* drawList, ImVec2 center, ImVec2 size, ImU32 color) {
                const ImVec2 textPos = ImVec2(center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f);
                drawList->AddText(textPos, color, label);
            }
        );
    }

    // Burger Icon Button
    static bool
    TactileBurgerButton(const char* str_id, const ImVec2& size_arg = ImVec2(0, 0), float rounding = DEFAULT_ROUNDING)
    {
        const float frameHeight = ImGui::GetFrameHeight();
        ImVec2 size = size_arg;
        if (size.x <= 0.0f)
            size.x = frameHeight;
        if (size.y <= 0.0f)
            size.y = frameHeight;

        return TactileButtonCore(
            str_id, size, rounding, [](ImDrawList* drawList, ImVec2 center, ImVec2 size, ImU32 color) {
                const float minDim = std::min(size.x, size.y);
                const float barWidth = minDim * BURGER_WIDTH_RATIO;
                const float barHeight = std::max(1.0f, minDim * BURGER_BAR_HEIGHT_RATIO);
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

    // Helper to standardize the 2-column property table layout
    static bool BeginPropertyRow(const char* label, float columnWidth)
    {
        ImGui::PushID(label);

        if (ImGui::BeginTable("##PropertyTable", 2, ImGuiTableFlags_SizingFixedFit))
        {
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

        return false;
    }

    static void EndPropertyRow()
    {
        ImGui::EndTable();
        ImGui::PopID();
    }
    // String Buffer Overload
    static bool InputField(const char* label, char* buffer, float columnWidth = 100.0f)
    {
        bool valueChanged = false;

        if (BeginPropertyRow(label, columnWidth))
        {
            // 1. Reset item spacing to ensure input frame padding aligns with DrawVec3 / DrawFloat
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{4.0f, 0.0f});

            // 2. Set width to match remaining available content region
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 30.0f);

            // 3. Hide ImGui's internal right-hand label
            std::string inputID = std::string("##") + label;

            if (ImGui::InputText(inputID.c_str(), buffer, sizeof(buffer)))
            {
                valueChanged = true;
            }

            ImGui::PopStyleVar();
            EndPropertyRow();
        }

        return valueChanged;
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
        bool valueChanged = false;

        if (BeginPropertyRow(label, columnWidth))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{4.0f, 0.0f});

            // Set control to stretch across remaining width
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 30.0f);

            if (ImGui::DragFloat("##value", &value, speed, min, max, format))
            {
                valueChanged = true;
            }

            // Right-click field to reset value
            if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
                value = resetValue;
                valueChanged = true;
            }

            ImGui::PopStyleVar();
            EndPropertyRow();
        }

        return valueChanged;
    }

    // Vector3 Control
    static bool DrawVec3Control(
        const char* label,
        glm::vec3& values,
        float resetValue = 0.0f,
        float columnWidth = 100.0f,
        float speed = 0.1f,
        const char* format = "%.2f"
    )
    {
        bool valueChanged = false;

        if (BeginPropertyRow(label, columnWidth))
        {
            constexpr float labelSpacing = 4.0f; // Spacing between label and input box
            constexpr float groupSpacing = 8.0f; // Spacing between X, Y, and Z groups

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{labelSpacing, 0.0f});

            ImGuiIO& io = ImGui::GetIO();
            ImFont* boldFont = io.Fonts->Fonts.Size > 0 ? io.Fonts->Fonts[0] : nullptr;

            // Calculate available width in column 1
            float availWidth = ImGui::GetContentRegionAvail().x - 30.0f;

            // Calculate text label sizes with font applied for accuracy
            if (boldFont)
                ImGui::PushFont(boldFont);
            float xLabelWidth = ImGui::CalcTextSize("X").x;
            float yLabelWidth = ImGui::CalcTextSize("Y").x;
            float zLabelWidth = ImGui::CalcTextSize("Z").x;
            if (boldFont)
                ImGui::PopFont();

            // Calculate fixed width taken up by text labels and spacing
            float nonInputWidth =
                (xLabelWidth + yLabelWidth + zLabelWidth) + (3.0f * labelSpacing) + (2.0f * groupSpacing);

            // Distribute remaining width equally among the 3 DragFloats
            float inputWidth = (availWidth - nonInputWidth) / 3.0f;
            if (inputWidth < 1.0f)
                inputWidth = 1.0f;

            auto DrawAxis =
                [&](const char* axisLabel, const char* imguiID, float& value, const ImVec4& color, bool isLast) {
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    if (boldFont)
                        ImGui::PushFont(boldFont);

                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted(axisLabel);

                    if (boldFont)
                        ImGui::PopFont();
                    ImGui::PopStyleColor();

                    ImGui::SameLine();

                    ImGui::SetNextItemWidth(inputWidth);
                    if (ImGui::DragFloat(imguiID, &value, speed, 0.0f, 0.0f, format))
                    {
                        valueChanged = true;
                    }

                    // Right-click axis label/field to reset value
                    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                    {
                        value = resetValue;
                        valueChanged = true;
                    }

                    if (!isLast)
                    {
                        ImGui::SameLine(0.0f, groupSpacing);
                    }
                };

            DrawAxis("X", "##X", values.x, ImVec4{0.85f, 0.25f, 0.25f, 1.0f}, false);
            DrawAxis("Y", "##Y", values.y, ImVec4{0.25f, 0.75f, 0.25f, 1.0f}, false);
            DrawAxis("Z", "##Z", values.z, ImVec4{0.25f, 0.45f, 0.90f, 1.0f}, true);

            ImGui::PopStyleVar();
            EndPropertyRow();
        }

        return valueChanged;
    }

    static void Checkbox(const std::string& name, bool& target, float size = 0.5f)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(size, size));
        ImGui::Checkbox(name.c_str(), &target);
        ImGui::PopStyleVar();
    }

    template <typename T, typename UIFunction>
    static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
    {
        ImGui::PushID(name.c_str());
        ImGuiTreeNodeFlags treeNodeFlags =
            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;
        if (entity.HasComponent<T>())
        {
            auto& component = entity.GetComponent<T>();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});

            ImGui::Separator();

            // Correct template type hashing for unique tree nodes
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name.c_str());
            ImGui::PopFont();
            const ImVec2 buttonSize = ImVec2(20.0f, 20.0f);

            // Align button to right edge
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - buttonSize.x);
            ImGui::PopStyleVar();

            if (UIUtils::TactileBurgerButton("test", buttonSize))
            {
                ImGui::OpenPopup("ComponentSettings");
            }

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
        }
        ImGui::PopID();
    }
    static bool MenuItem(const char* label)
    {
        // 1. Add extra vertical and horizontal padding to each MenuItem
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
        bool clicked = ImGui::MenuItem(label);
        ImGui::PopStyleVar(2);
        return clicked;
    }
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
        bool valueChanged = false;

        if (BeginPropertyRow(label, columnWidth))
        {
            const int currentIndex = static_cast<int>(currentEnumValue);
            const char* currentLabel =
                (currentIndex >= 0 && currentIndex < optionCount) ? optionStrings[currentIndex] : "Unknown";

            // Make combo stretch across remaining column width
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 30.0f);

            // Use "##" prefix to hide ImGui's default right-side label
            std::string comboID = std::string("##") + label;
            if (ImGui::BeginCombo(comboID.c_str(), currentLabel))
            {
                for (int i = 0; i < optionCount; i++)
                {
                    const bool isSelected = (currentIndex == i);
                    if (ImGui::Selectable(optionStrings[i], isSelected))
                    {
                        currentEnumValue = static_cast<EnumType>(i);
                        valueChanged = true;
                    }

                    // Set initial focus to current selection when popup opens
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            EndPropertyRow();
        }

        return valueChanged;
    }
    static void CenterElement(float elementWidth)
    {
        float availableWidth = ImGui::GetContentRegionAvail().x;
        float offsetX = (availableWidth - elementWidth) * 0.5f;

        if (offsetX > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
    }
    static void PanelContentSeparator() { ImGui::Dummy(ImVec2(0.0f, 1.0f)); }
};

} // namespace Oven