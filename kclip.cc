#include "kclip.hh"

#include <stdio.h>

TheUniverse G = { };

static void file_select_task(void*)
{
}

void kclip_init1_before_window()
{
  const char* selected_file = (G.argc >= 2) ? G.argv[1] : NULL;

  // If no file was supplied, prompt the user the a file. Run the file selection code in
  // another thread so we can use the time to set up the window
  if (!selected_file) {
    create_thread(&G.file_select_thread, file_select_task, NULL);
  }
}

void kclip_init2_setup_dear_imgui()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = NULL;

#ifdef K_WINDOWS
  io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 18.0f);
#endif
}

void kclip_init3_before_window_shown()
{
  // @@ await file select response
}

void kclip_frame()
{
  ImGui::NewFrame();

  ImGui::ShowDemoWindow();

  ImGui::Render();
}

