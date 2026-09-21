#pragma once
/*

#define COLOR_PORCELAIN_WHITE ImVec4(0.941f, 0.957f, 0.969f, 1.00f)     // Primary Background / Canvas
#define COLOR_CHARCOAL_DARK ImVec4(0.169f, 0.176f, 0.188f, 1.00f)       // Primary Text & Contrast Borders
#define COLOR_SLATE_TRIM ImVec4(0.345f, 0.349f, 0.357f, 1.00f)          // Structural Frame & Outer Borders
#define COLOR_ACCENT_CYAN ImVec4(0.000f, 0.643f, 0.894f, 0.7f)          // Primary Focus / Selection Accent
#define COLOR_ACCENT_ORANGE ImVec4(1.000f, 0.400f, 0.000f, 0.7f)        // Secondary Highlight / Active State Accent
#define COLOR_ACCENT_ORANGE_LIGHT ImVec4(1.000f, 0.700f, 0.300f, 1.00f) // Secondary Highlight / Active State Accent
#define COLOR_INDICATOR_CYAN ImVec4(0.212f, 0.753f, 0.945f, 1.00f)      // Bright Interactive Highlight
#define COLOR_SLATE_MUTED ImVec4(0.282f, 0.322f, 0.353f, 1.00f)         // Secondary Inactive State
#define COLOR_ALERT_RED ImVec4(0.827f, 0.184f, 0.184f, 1.00f)           // Danger / Warning Indicator
#define COLOR_DEEP_CHARCOAL ImVec4(0.118f, 0.125f, 0.133f, 1.00f)       // Deep Shadow / Recessed Surface
#define COLOR_MUTED_GREEN ImVec4(0.227f, 0.294f, 0.157f, 1.00f)         // Plot / Graph Status Line
#define COLOR_COOL_WHITE ImVec4(0.868f, 0.899f, 0.923f, 1.00f)          // Field Background / Recessed Lighting
#define COLOR_WARM_AMBER ImVec4(0.902f, 0.631f, 0.361f, 1.00f)          // Muted Highlight Accent
#define COLOR_STEEL_GRAY ImVec4(0.578f, 0.610f, 0.633f, 1.00f)          // Subdued Surface Metal
#define COLOR_HOVER_CYAN ImVec4(0.318f, 0.769f, 0.929f, 0.5f)           // Interactive Hover Highlight


#define COLOR_TEXT_DEFAULT COLOR_CHARCOAL_DARK
#define COLOR_TEXT_DISABLED COLOR_STEEL_GRAY
#define COLOR_TEXT_SELECTED COLOR_HOVER_CYAN

#define COLOR_INPUT_FIELD_DEFAULT COLOR_PORCELAIN_WHITE
#define COLOR_INPUT_FIELD_HOVERED COLOR_COOL_WHITE

#define COLOR_PANEL_DEFAULT COLOR_COOL_WHITE

#define COLOR_MENU_BAR COLOR_COOL_WHITE

#define COLOR_TAB_HEADER_BG COLOR_STEEL_GRAY
#define COLOR_TAB_TITLE_BG COLOR_COOL_WHITE

#define COLOR_HOVERED_DEFAULT COLOR_ACCENT_ORANGE_LIGHT
#define COLOR_FOCUSED_DEFAULT COLOR_ACCENT_ORANGE

#define COLOR_CHECKMARK COLOR_ACCENT_ORANGE

#define COLOR_SLIDER COLOR_SLATE_MUTED
#define COLOR_SCROLLBAR_BG COLOR_PORCELAIN_WHITE
#define COLOR_SCROLLBAR COLOR_STEEL_GRAY
*/

// ============================================================================
// PALETTE SOMBRE STYLE GOLDSRC / HAMMER EDITOR
// Référence : HUD Half-Life (255,160,0), viewports 2D de Valve Hammer 3.x,
// menus GoldSrc (gris chauds désaturés, jamais bleutés).
//
// Seul ce fichier connaît les COLOR_GOLDSRC_*. L'application du thème ne
// consomme que les alias de la seconde section.
// ============================================================================

// Surfaces & Fonds
#define COLOR_GOLDSRC_BG_DEEP ImVec4(0.047f, 0.047f, 0.043f, 1.00f)       // Noir viewport 2D Hammer
#define COLOR_GOLDSRC_BG_CANVAS ImVec4(0.106f, 0.102f, 0.094f, 1.00f)     // Fenêtres & Canvas (gris chaud très sombre)
#define COLOR_GOLDSRC_SURFACE_MUTED ImVec4(0.071f, 0.071f, 0.063f, 1.00f) // Surface encastrée (champs de saisie)
#define COLOR_GOLDSRC_PANEL ImVec4(0.180f, 0.173f, 0.157f, 1.00f)         // Panneaux & en-têtes (surface en relief)
#define COLOR_GOLDSRC_PANEL_LIGHT ImVec4(0.255f, 0.243f, 0.220f, 1.00f)   // Panneau survolé / scrollbar

// Bordures & Structure (biseau 3D façon Win95 / Hammer)
#define COLOR_GOLDSRC_BORDER ImVec4(0.294f, 0.282f, 0.255f, 1.00f)      // Cadres & séparateurs
#define COLOR_GOLDSRC_BEVEL_LIGHT ImVec4(0.435f, 0.420f, 0.384f, 1.00f) // Arête haute du biseau
#define COLOR_GOLDSRC_BEVEL_DARK ImVec4(0.031f, 0.031f, 0.027f, 1.00f)  // Arête basse / ombre portée
#define COLOR_GOLDSRC_STEEL ImVec4(0.510f, 0.494f, 0.451f, 1.00f)       // Métal clair / élément secondaire

// Textes
#define COLOR_GOLDSRC_TEXT ImVec4(0.882f, 0.867f, 0.827f, 1.00f)       // Blanc osseux (pas de blanc pur froid)
#define COLOR_GOLDSRC_TEXT_MUTED ImVec4(0.522f, 0.506f, 0.467f, 1.00f) // Texte désactivé / secondaire

// Couleurs d'accentuation (HUD GoldSrc)
#define COLOR_GOLDSRC_AMBER ImVec4(1.000f, 0.627f, 0.000f, 1.00f)        // 255,160,0 — ambre HUD exact
#define COLOR_GOLDSRC_AMBER_BRIGHT ImVec4(1.000f, 0.780f, 0.361f, 1.00f) // Halo / liseré lumineux
#define COLOR_GOLDSRC_AMBER_DIM ImVec4(0.612f, 0.376f, 0.024f, 1.00f)    // Ambre éteint (survol, onglet actif)
#define COLOR_GOLDSRC_AMBER_PRESS ImVec4(1.000f, 0.627f, 0.000f, 0.45f)  // Voile ambré translucide
#define COLOR_GOLDSRC_ORANGE ImVec4(1.000f, 0.365f, 0.055f, 1.00f)       // Alerte / focus fort
#define COLOR_GOLDSRC_GREEN_MUTED ImVec4(0.404f, 0.549f, 0.322f, 1.00f)  // Grille Hammer / tracé console
#define COLOR_GOLDSRC_GREEN_BRIGHT ImVec4(0.573f, 0.769f, 0.400f, 1.00f) // Tracé actif / valeur OK
#define COLOR_GOLDSRC_ALERT_RED ImVec4(0.859f, 0.149f, 0.106f, 1.00f)    // Santé basse HL / DANGER

// Utilitaires neutres
#define COLOR_GOLDSRC_NONE ImVec4(0.000f, 0.000f, 0.000f, 0.00f)       // Transparent
#define COLOR_GOLDSRC_ROW_ALT ImVec4(1.000f, 1.000f, 1.000f, 0.03f)    // Zébrage de tableau
#define COLOR_GOLDSRC_DIM_SOFT ImVec4(0.000f, 0.000f, 0.000f, 0.35f)   // Assombrissement modal
#define COLOR_GOLDSRC_DIM_STRONG ImVec4(0.000f, 0.000f, 0.000f, 0.50f) // Assombrissement Ctrl+Tab

// ============================================================================
// ALIAS DE CONFIGURATION DU THÈME
// ============================================================================

#define COLOR_TEXT_DEFAULT COLOR_GOLDSRC_TEXT
#define COLOR_TEXT_DISABLED COLOR_GOLDSRC_TEXT_MUTED
// Alimente à la fois TextSelectedBg et InputTextCursor : garder un alpha,
// sinon le fond de sélection recouvre les glyphes.
#define COLOR_TEXT_SELECTED COLOR_GOLDSRC_AMBER_PRESS
#define COLOR_TEXT_LINK COLOR_GOLDSRC_AMBER

#define COLOR_INPUT_FIELD_DEFAULT COLOR_GOLDSRC_SURFACE_MUTED
#define COLOR_INPUT_FIELD_HOVERED COLOR_GOLDSRC_PANEL

#define COLOR_PANEL_DEFAULT COLOR_GOLDSRC_BG_CANVAS

#define COLOR_MENU_BAR COLOR_GOLDSRC_PANEL

#define COLOR_TAB_BG COLOR_GOLDSRC_BG_DEEP // Onglet inactif (manquait)
#define COLOR_TAB_HEADER_BG COLOR_GOLDSRC_PANEL
#define COLOR_TAB_TITLE_BG COLOR_GOLDSRC_BG_CANVAS
#define COLOR_TAB_OVERLINE COLOR_GOLDSRC_AMBER_BRIGHT // Liseré de l'onglet sélectionné
#define COLOR_TAB_OVERLINE_DIMMED COLOR_GOLDSRC_AMBER_DIM

// Opaques : ces deux alias servent aussi de poignée de slider et de liseré de
// navigation, où un voile translucide ne se voit pas.
#define COLOR_HOVERED_DEFAULT COLOR_GOLDSRC_AMBER_DIM
#define COLOR_FOCUSED_DEFAULT COLOR_GOLDSRC_AMBER

#define COLOR_CHECKMARK COLOR_GOLDSRC_AMBER

#define COLOR_SLIDER COLOR_GOLDSRC_STEEL
#define COLOR_SCROLLBAR_BG COLOR_GOLDSRC_BG_DEEP
#define COLOR_SCROLLBAR COLOR_GOLDSRC_PANEL_LIGHT

#define COLOR_BUTTON_DEFAULT COLOR_GOLDSRC_PANEL

// --- Slots qui manquaient et retombaient sur le bleu par défaut ---
#define COLOR_BORDER_DEFAULT COLOR_GOLDSRC_BORDER
#define COLOR_BORDER_SHADOW COLOR_GOLDSRC_NONE
#define COLOR_SEPARATOR COLOR_GOLDSRC_BORDER
#define COLOR_SEPARATOR_HOVERED COLOR_GOLDSRC_AMBER_DIM
#define COLOR_SEPARATOR_ACTIVE COLOR_GOLDSRC_AMBER

#define COLOR_RESIZE_GRIP COLOR_GOLDSRC_BORDER

#define COLOR_TABLE_HEADER_BG COLOR_GOLDSRC_PANEL
#define COLOR_TABLE_BORDER_STRONG COLOR_GOLDSRC_BORDER
#define COLOR_TABLE_BORDER_LIGHT COLOR_GOLDSRC_BEVEL_DARK
#define COLOR_TABLE_ROW_BG COLOR_GOLDSRC_NONE
#define COLOR_TABLE_ROW_BG_ALT COLOR_GOLDSRC_ROW_ALT

#define COLOR_PLOT_LINE COLOR_GOLDSRC_GREEN_MUTED
#define COLOR_PLOT_LINE_HOVERED COLOR_GOLDSRC_GREEN_BRIGHT
#define COLOR_PLOT_HISTOGRAM COLOR_GOLDSRC_AMBER
#define COLOR_PLOT_HISTOGRAM_HOVERED COLOR_GOLDSRC_AMBER_BRIGHT

#define COLOR_DOCKING_PREVIEW COLOR_GOLDSRC_AMBER_PRESS
#define COLOR_DOCKING_EMPTY_BG COLOR_GOLDSRC_BG_DEEP

#define COLOR_NAV_WINDOWING_HIGHLIGHT COLOR_GOLDSRC_AMBER_BRIGHT
#define COLOR_NAV_WINDOWING_DIM_BG COLOR_GOLDSRC_DIM_STRONG
#define COLOR_DRAG_DROP_TARGET COLOR_GOLDSRC_ORANGE
#define COLOR_MODAL_DIM_BG COLOR_GOLDSRC_DIM_SOFT

#define COLOR_ALERT COLOR_GOLDSRC_ALERT_RED