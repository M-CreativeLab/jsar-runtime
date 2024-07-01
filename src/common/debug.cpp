#include <stdarg.h>
#include "debug.hpp"

#ifdef _WIN32
#include <windows.h>
#include <processthreadsapi.h>
#else
#include <dlfcn.h>
#ifdef __ANDROID__
#include <sys/prctl.h>
#include <cxxabi.h>
#include <unwind.h>
#endif
#endif

#ifndef TRANSMUTE_STANDALONE
void SET_UNITY_LOG_HANDLE(IUnityLog *log)
{
  s_UnityLog = log;
}
IUnityLog *GET_UNITY_LOG_HANDLE()
{
  return s_UnityLog;
}
#endif

void DEBUG(const char *tag, const char *format, ...)
{
  va_list args;
  va_start(args, format);
#ifdef __ANDROID__
  __android_log_vprint(ANDROID_LOG_DEBUG, tag, format, args);
#else
  char buffer[1024];
  int length = vsnprintf(nullptr, 0, format, args);
  if (length > 1024)
    length = 1024;
  vsnprintf(buffer, length + 1, format, args);

  if (s_UnityLog == nullptr)
    fprintf(stdout, "[%s] %s\n", tag, buffer);
  else
    UNITY_LOG(s_UnityLog, buffer);
#endif
  va_end(args);
}

void SET_THREAD_NAME(const std::string &name)
{
#ifdef _WIN32
  // Windows
  SetThreadDescription(GetCurrentThread(), std::wstring(name.begin(), name.end()).c_str());
#elif __ANDROID__
  // Android
  prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
#elif __APPLE__
  // POSIX systems (Linux, macOS)
  pthread_setname_np(name.c_str());
#endif
}

#ifdef __ANDROID__
struct android_backtrace_state
{
  void **current;
  void **end;
};
_Unwind_Reason_Code android_unwind_callback(struct _Unwind_Context *ctx, void *arg)
{
  android_backtrace_state *state = (android_backtrace_state *)arg;
  uintptr_t pc = _Unwind_GetIP(ctx);
  if (pc)
  {
    if (state->current == state->end)
      return _URC_END_OF_STACK;
    else
      *state->current++ = reinterpret_cast<void *>(pc);
  }
  return _URC_NO_REASON;
}
#endif

void printsStacktraceOnSignal(int signal)
{
  DEBUG(LOG_TAG_ERROR, "Received SIGNAL (%d), printing backtrace:", signal);
#ifdef __APPLE__
  const int maxFrames = 20;
  void *stackTrace[maxFrames];
  int numFrames = backtrace(stackTrace, maxFrames);
  char **symbols = backtrace_symbols(stackTrace, numFrames);
  if (symbols == nullptr)
  {
    fprintf(stderr, "Failed to obtain backtrace symbols");
  }
  else
  {
    for (int i = 0; i < numFrames; ++i)
      fprintf(stderr, "%s\n", symbols[i]);
    free(symbols);
  }
#elif __ANDROID__
  const int max = 512;
  void *buffer[max];
  android_backtrace_state state;
  state.current = buffer;
  state.end = buffer + max;

  _Unwind_Backtrace(android_unwind_callback, &state);
  int backtraceCount = (int)(state.current - buffer);
  if (backtraceCount == 0)
  {
    std::cout << "Failed to obtain backtrace symbols" << std::endl;
  }
  else
  {
    for (int n = 0; n < backtraceCount; n++)
    {
      const void *addr = buffer[n];
      const char *symbol = "";

      Dl_info info;
      if (dladdr(addr, &info) && info.dli_sname)
      {
        symbol = info.dli_sname;
        int status = 0;
        char *demangled = __cxxabiv1::__cxa_demangle(symbol, 0, 0, &status);
        fprintf(stdout, "  #%d pc %p %s",
                n, addr,
                status == 0 && demangled != nullptr ? demangled : symbol);

        if (demangled != nullptr)
          free(demangled);
      }
    }
  }
#endif
  _exit(EXIT_FAILURE);
}

constexpr int SIGNALS[] = {
    SIGABRT,
    SIGFPE,
    SIGSEGV,
#ifndef _WIN32
    SIGHUP,
    SIGQUIT,
    SIGBUS,
    SIGSYS,
    SIGPIPE,
#endif
};

void ENABLE_BACKTRACE()
{
  for (int id : SIGNALS)
    signal(id, printsStacktraceOnSignal);
}
