#ifndef _KCLIP_HH_
#define _KCLIP_HH_

#include "kbase.hh"

#include "imgui.h"

// ================================================================================
// App constants
// ================================================================================

constexpr const char* KCLIP_TITLE = "KClip";
constexpr u16         KCLIP_W     = 1024;
constexpr u16         KCLIP_H     = 768;

// ================================================================================
// UI theme
// ================================================================================

enum : u32
{
  UI_COLOR_BG = 0,
  UI_COLOR_WIDGET_BG,
  UI_COLOR_WIDGET,
  UI_COLOR_WIDGET_FOCUS,

  UI_COLOR_WIDGET_PRIME,
  UI_COLOR_WIDGET_PRESS,

  _UI_COLOR_COUNT,
};

enum : u32
{
  UI_PAD_WIDGET = 0,

  _UI_PAD_COUNT,
};

enum : u32
{
  UI_TINT_DULL = 0,
  UI_TINT_SHINY,

  _UI_TINT_COUNT,
};

struct Theme
{
  ImColor colors[_UI_COLOR_COUNT];
  ImVec2  padding[_UI_PAD_COUNT];
  i16     tint[_UI_TINT_COUNT];
};

// ================================================================================
// Shared app code
// ================================================================================

void KClip_InitBeforeWindow();
void KClip_DrawUI();

// ================================================================================
// Custom Dear ImGui widgets
// ================================================================================

namespace ImGui
{
  // Draw a generic pane without advancing the cursor
  void KRawPane(const ImVec2& p_min, const ImVec2& p_max, ImColor col, bool inset = false);

  // Button with a label
  bool KButton(const char* label);
}

// ================================================================================
// The Universe
// ================================================================================

struct TheUniverse
{
  Theme theme;
};

extern TheUniverse G;

#endif // _KCLIP_HH_
