#include "kclip.hh"

#include <Cocoa/Cocoa.h>

static struct
{
  NSWindow* wnd;
} MAC = { };

void create_window_and_init_graphics(void)
{
  @autoreleasepool {
    NSRect frame = NSMakeRect(160, 120, DEFAULT_WINDOW_W, DEFAULT_WINDOW_H);
    NSWindowStyleMask style = NSWindowStyleMaskTitled |
                              NSWindowStyleMaskClosable |
                              NSWindowStyleMaskMiniaturizable |
                              NSWindowStyleMaskResizable;
    MAC.wnd = [[NSWindow alloc] initWithContentRect:frame
                                          styleMask:style
                                            backing:NSBackingStoreBuffered 
                                              defer:NO];
    MAC.wnd.title = @"kclip";
  }
}

void show_window(void)
{
  DBG_ASSERT(MAC.wnd);

  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
  [NSApp finishLaunching];

  [MAC.wnd center];
  [MAC.wnd makeKeyAndOrderFront:nil];
  [MAC.wnd orderFrontRegardless];
  [NSApp activateIgnoringOtherApps:YES];
  // [NSApp run];
}

void select_media_file(void)
{
}

int main(int argc, const char* argv[])
{
  kclip_main(argc, argv);
  return EXIT_SUCCESS;
}
