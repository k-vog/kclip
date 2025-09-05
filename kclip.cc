#include "kclip.hh"

// ================================================================================
// The Universe
// ================================================================================

TheUniverse G = { };

// ================================================================================
// Shared app code
// ================================================================================

static inline ImColor ImColor_Broadcast(u8 val)
{
  return ImColor(val, val, val);
}

void KClip_InitBeforeWindow()
{
  G.theme.colors[UI_COLOR_BG          ] = ImColor_Broadcast(0x17);
  G.theme.colors[UI_COLOR_WIDGET_BG   ] = ImColor_Broadcast(0x20);
  G.theme.colors[UI_COLOR_WIDGET      ] = ImColor_Broadcast(0x40);
  G.theme.colors[UI_COLOR_WIDGET_FOCUS] = ImColor_Broadcast(0x47);

  G.theme.colors[UI_COLOR_WIDGET_PRIME] = ImColor_Broadcast(0x50);
  G.theme.colors[UI_COLOR_WIDGET_PRESS] = ImColor_Broadcast(0x40);

  G.theme.padding[UI_PAD_WIDGET] = ImVec2(5.0f, 3.0f);

  G.theme.tint[UI_TINT_DULL ] = -0x07;
  G.theme.tint[UI_TINT_SHINY] =  0x17;
}

void KClip_DrawUI()
{
  ImGuiViewport* vp = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(vp->WorkPos);
  ImGui::SetNextWindowSize(vp->WorkSize);
  ImGuiWindowFlags wflags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
  if (ImGui::Begin("TEST WINDOW", 0, wflags)) {
    ImGui::KRawPane(ImVec2(100, 100), ImVec2(500, 500), G.theme.colors[UI_COLOR_WIDGET]);
    ImGui::KRawPane(ImVec2(120, 120), ImVec2(480, 480), G.theme.colors[UI_COLOR_WIDGET_BG], true);
    ImGui::SetCursorPos(ImVec2(125, 125));
    if (ImGui::KButton("Test button 1!")) {
    }
    ImGui::SameLine();
    //ImGui::Button("Test button 2!");
    ImGui::End();
  }
}
