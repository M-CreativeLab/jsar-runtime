#include <stdarg.h>
#include "debug.hpp"

#ifdef _WIN32
#include <windows.h>
#include <processthreadsapi.h>
#elif __ANDROID__
#include <sys/prctl.h>
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
  if (
      strcmp(tag, "TR_GLES") == 0 ||
      strcmp(tag, "TR_RAPI") == 0)
    return;

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
    fprintf(stdout, "[#%d](%s) %s\n", getpid(), tag, buffer);
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

void printsStacktraceOnSignal(int signal)
{
#ifdef __APPLE__
  const int maxFrames = 20;
  void *stackTrace[maxFrames];
  int numFrames = backtrace(stackTrace, maxFrames);
  char **symbols = backtrace_symbols(stackTrace, numFrames);
  if (symbols == nullptr)
  {
    std::cerr << "Failed to obtain backtrace symbols" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cerr << "Received signal " << signal << ", printing stack trace:" << std::endl;
  for (int i = 0; i < numFrames; ++i)
    std::cerr << symbols[i] << std::endl;
  free(symbols);
#endif
  _exit(EXIT_FAILURE);
}

constexpr int SIGNALS[] = {
    SIGHUP,
    SIGQUIT,
    SIGABRT,
    SIGFPE,
    SIGBUS,
    SIGSEGV,
    SIGSYS,
    SIGPIPE,
};

void ENABLE_BACKTRACE()
{
  for (int id : SIGNALS)
    signal(id, printsStacktraceOnSignal);
}
