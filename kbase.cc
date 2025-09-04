#include "kbase.hh"

// ================================================================================
// Core helpers
// ================================================================================

const char* TempFmt(const char* fmt, ...)
{
  static thread_local char buf[1024];
  va_list va; va_start(va, fmt);
  vsnprintf(buf, sizeof(buf), fmt, va);
  va_end(va);
  return buf;
}

