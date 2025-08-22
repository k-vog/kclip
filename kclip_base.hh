#ifndef _KCLIP_BASE_HH_
#define _KCLIP_BASE_HH_

//
// Platform detection
//

#ifdef _WIN32
# define K_WINDOWS
#endif

#ifdef __APPLE__
# define K_APPLE
#endif

#ifdef __linux__
# define K_LINUX
#endif

#if defined(K_APPLE) || defined(K_LINUX)
# define K_POSIX
#endif

//
// Other headers
//

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef K_WINDOWS
# include <windows.h>
#endif

#ifdef K_POSIX
# include <pthread.h>
#endif

//
// Core types
//

using u8  = std::uint8_t;
using u32 = std::uint32_t;

using f32 = float;

using usize = std::size_t;

//
// Core helpers
//

#define DBG_ASSERT(...) assert(__VA_ARGS__)

//
// Wrappers around C functions in case we ever swap them out
//

#define K_strdup strdup

//
// Hacky defer macro thingy via C++ RAII
//
// ref: https://veg.by/en/blog/2024/08/03/defer-macro/ 
//

#define DEFER_CONCAT_(x, y) x ## y
#define DEFER_CONCAT(x, y) DEFER_CONCAT_(x, y)

template <typename T>
struct deferrer
{
  T f;
  deferrer(T f) : f(f) { };
  deferrer(const deferrer&) = delete;
  ~deferrer() { f(); }
};

#define kdefer deferrer DEFER_CONCAT(_defer_, __LINE__) = [&]

//
// Threads
//

using ThreadProc = void(*)(void* opaque);

struct Thread
{
#ifdef K_WINDOWS
  HANDLE      handle;
#endif
#ifdef K_POSIX
  pthread_t   tid;
#endif
  ThreadProc  proc;
  void*       opaque;
};

#ifdef K_WINDOWS

static inline DWORD winthread_proc_caller(LPVOID p)
{
  Thread* t = (Thread*)p;
  t->proc(t->opaque);
  return 0;
}

#endif

#ifdef K_POSIX

static inline void* pthread_proc_caller(void* p)
{
  Thread* t = (Thread*)p;
  t->proc(t->opaque);
  return NULL;
}

#endif

static inline void create_thread(Thread* t, ThreadProc proc, void* opaque)
{
  DBG_ASSERT(t && proc);
  t->proc = proc;
  t->opaque = opaque;
#ifdef K_WINDOWS
  t->handle = CreateThread(NULL, 0, winthread_proc_caller, t, 0, NULL);
  DBG_ASSERT(t->handle);
#endif
#ifdef K_POSIX
  int ret = pthread_create(&t->tid, NULL, pthread_proc_caller, t);
  DBG_ASSERT(ret == 0);
#endif
}

#endif // _KCLIP_BASE_HH_
