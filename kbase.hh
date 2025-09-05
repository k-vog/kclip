#ifndef _KCLIP_BASE_HH_
#define _KCLIP_BASE_HH_

// ================================================================================
// Other headers
// ================================================================================

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// ================================================================================
// Core types
// ================================================================================

using i16 = int16_t;
using i32 = int32_t;
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

using f32 = float;

using usz = size_t;

// ================================================================================
// Core helpers
// ================================================================================

#define ASSERT(...) assert(__VA_ARGS__)

#define SPEW(...) printf("kclip: " __VA_ARGS__); printf("\n"); fflush(stdout);

template <typename T, usz N>
constexpr usz ArrayLen(T(&)[N])
{
  return N;
}

template <typename T>
static inline bool InRange(T xmin, T xmax, T x)
{
  return x >= xmin && x <= xmax;
}

template <typename T>
static inline T Clamp(T xmin, T xmax, T x)
{
  if (x > xmax) {
    return xmax;
  }
  if (x < xmin) {
    return xmin;
  }
  return x;
}

// ================================================================================
// String helpers
// ================================================================================

const char* TempFmt(const char* fmt, ...);

#endif // _KCLIP_BASE_HH_

