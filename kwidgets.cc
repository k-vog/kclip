#include "kclip.hh"

#include "imgui_internal.h"

// ================================================================================
// Custom Dear ImGui widgets
// ================================================================================

static ImColor CalcTint(ImColor color, i16 tint8)
{
  ASSERT(InRange<i16>(-128, 128, tint8));
  const f32 f_tint = (f32)tint8 / 128.0f;
  const f32 r = Clamp(0.0f, 1.0f, color.Value.x + f_tint);
  const f32 g = Clamp(0.0f, 1.0f, color.Value.y + f_tint);
  const f32 b = Clamp(0.0f, 1.0f, color.Value.z + f_tint);
  return ImColor(r, g, b, color.Value.w);
}

void ImGui::KRawPane(const ImVec2& p_min, const ImVec2& p_max, ImColor col, bool inset)
{
  const ImRect rect = ImRect(p_min, p_max);
  const ImColor col_dull = CalcTint(col, G.theme.tint[UI_TINT_DULL]);
  const ImColor col_shiny = CalcTint(col, G.theme.tint[UI_TINT_SHINY]);
  ImGui::GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, col);
  const f32 edge = 1.0f;
  if (inset) {
    ImGui::GetWindowDrawList()->AddRectFilled(rect.GetTL(), rect.GetTR() + ImVec2(0.0f, edge), col_dull);
    ImGui::GetWindowDrawList()->AddRectFilled(rect.GetBL(), rect.GetBR() - ImVec2(0.0f, edge), col_dull);
    ImGui::GetWindowDrawList()->AddRectFilled(rect.GetTL(), rect.GetBL() + ImVec2(edge, 0.0f), col_dull);
    ImGui::GetWindowDrawList()->AddRectFilled(rect.GetTR(), rect.GetBR() - ImVec2(edge, 0.0f), col_dull);
  }
  else {
    ImGui::GetWindowDrawList()->AddRectFilled(rect.GetTL(), rect.GetTR() + ImVec2(0.0f, edge), col_shiny);
    //ImGui::GetWindowDrawList()->AddRectFilled(rect.GetBL(), rect.GetBR() - ImVec2(0.0f, edge), col_dull);
  }
}

bool ImGui::KButton(const char* label)
{
  ImGuiWindow* wnd = ImGui::GetCurrentWindow();
  if (wnd->SkipItems) {
    return false;
  }

  ImVec2 pad = G.theme.padding[UI_PAD_WIDGET];

  ImVec2 label_size = ImGui::CalcTextSize(label, 0, true);
  ImVec2 size = label_size + (pad * 2.0f);

  ImGuiID id  = wnd->GetID(label);

  ImVec2 cur = wnd->DC.CursorPos;
  ImRect bb = ImRect(cur, cur + size);
  ImGui::ItemSize(bb);
  if (!ImGui::ItemAdd(bb, wnd->GetID(label))) {
    return false;
  }

  bool hovered = false;
  bool held    = false;
  bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

  usz col_idx = UI_COLOR_WIDGET;
  if (hovered) {
    col_idx = UI_COLOR_WIDGET_PRIME;
  }
  if (held) {
    col_idx = UI_COLOR_WIDGET_PRESS;
  }
  ImGui::KRawPane(bb.Min, bb.Max, G.theme.colors[col_idx], held);

  ImVec2 text_pos = bb.GetCenter() - label_size * 0.5f;
  ImGui::GetWindowDrawList()->AddText(text_pos, IM_COL32_WHITE, label);

  return pressed;
}
