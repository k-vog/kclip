#ifndef _KCLIP_H_
#define _KCLIP_H_

#include "kclip_base.hh"

//
// The Universe
//

struct TheUniverse
{
  usize         argc;
  const char**  argv;

  Thread        file_select_thread;
};

extern TheUniverse G;

//
// Application
//

void kclip_main(usize argc, const char* argv[]);

//
// OS layer implements these
//

static constexpr u32 DEFAULT_WINDOW_W = 1024;
static constexpr u32 DEFAULT_WINDOW_H = 768;

void create_window_and_init_graphics(void);
void show_window(void);

void select_media_file(void);

#endif // _KCLIP_H_
