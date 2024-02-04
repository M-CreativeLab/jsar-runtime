#include <stdarg.h>
#include "debug.hpp"
#include "logger.hpp"

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
  va_list args;
  va_start(args, format);

  int length = vsnprintf(nullptr, 0, format, args);
  char *buffer = new char[length + 1];

  va_end(args);
  va_start(args, format);
  vsnprintf(buffer, length + 1, format, args);

#ifdef __ANDROID__
  __android_log_print(ANDROID_LOG_DEBUG, tag, "%s", buffer);
#else
  fprintf(stdout, "[%s]: %s\n", tag, buffer);
#ifndef TRANSMUTE_STANDALONE
  if (s_UnityLog != nullptr)
    UNITY_LOG(s_UnityLog, buffer);
#endif
#endif

  delete[] buffer;
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
