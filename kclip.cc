#include "kclip.hh"

#include <stdio.h>

TheUniverse G = { };

static void file_select_task(void*)
{
}

void kclip_main(usize argc, const char* argv[])
{
  G.argc = argc;
  G.argv = argv;
  
  const char* selected_file = (G.argc >= 2) ? G.argv[1] : NULL;

  // If no file was supplied, prompt the user the a file. Run the file selection code in
  // another thread so we can use the time to set up the window
  if (!selected_file) {
    create_thread(&G.file_select_thread, file_select_task, NULL);
  }

  create_window_and_init_graphics();

  if (!selected_file) {
    // @@ await response
  }

  show_window();
}
