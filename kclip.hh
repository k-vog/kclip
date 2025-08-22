#ifndef _KCLIP_H_
#define _KCLIP_H_

#include "kclip_base.hh"

#include "imgui.h"

//
// The Universe
//

constexpr u32 APP_STATE_QUIT_REQUESTED = 1 << 0;

struct TheUniverse
{
  usize         argc;
  const char**  argv;

  u32           app_state;

  Thread        file_select_thread;
};

extern TheUniverse G;

//
// Application
//

void kclip_init1_before_window();
void kclip_init2_setup_dear_imgui();
void kclip_init3_before_window_shown();

void kclip_frame();

//
// OS layer implements these
//

static constexpr u32 DEFAULT_WINDOW_W = 1024;
static constexpr u32 DEFAULT_WINDOW_H = 768;

// sync
void select_media_file(void);

#endif // _KCLIP_H_
