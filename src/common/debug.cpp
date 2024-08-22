#include <stdarg.h>
#include <ctime>
#include <chrono>
#include <string>
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
#elif __APPLE__
#include <pthread.h>
#include <TargetConditionals.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>
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

using system_clock = std::chrono::system_clock;
using milliseconds = std::chrono::milliseconds;

inline std::string
fetchTimestamp()
{
  using namespace std::chrono;

  auto now = system_clock::now();
  auto now_c = system_clock::to_time_t(now);
  auto ms = std::chrono::duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

  struct tm tm_info;
  localtime_r(&now_c, &tm_info);

  char timestamp[32];
  strftime(timestamp, sizeof(timestamp), "%m-%d %H:%M:%S", &tm_info);
  snprintf(timestamp + 14, sizeof(timestamp) - 14, ".%03d", static_cast<int>(ms.count()));
  return std::string(timestamp);
}

void DEBUG(const char *tag, const char *format, ...)
{
  va_list args;
  va_start(args, format);
#ifdef __ANDROID__
  __android_log_vprint(ANDROID_LOG_DEBUG, tag, format, args);
#else
  fprintf(stdout, "%s ", fetchTimestamp().c_str());
  vfprintf(stdout, format, args);
  fprintf(stdout, "\n");
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

/**
 * Copied from "https://github.com/openwebos/nodejs/blob/master/src/platform_darwin_proctitle.cc".
 * 
 * NOTE: This doesn't work for forked process either, but it won't crash.
 * TODO: Supports for forked process namely applications.
 */
#ifdef __APPLE__
void _SetProcessTitleOnDarwin(const std::string &name)
{
  static int symbol_lookup_status = 0; // 1 = ok, 2 = unavailable
  if (symbol_lookup_status == 2) {
    // feature is unavailable
    return;
  }
  const char *title = name.c_str();

  // Warning: here be dragons! This is SPI reverse-engineered from WebKit's
  // plugin host, and could break at any time (although realistically it's only
  // likely to break in a new major release).
  // When 10.7 is available, check that this still works, and update this
  // comment for 10.8.

  // Private CFType used in these LaunchServices calls.
  typedef CFTypeRef PrivateLSASN;
  typedef PrivateLSASN (*LSGetCurrentApplicationASNType)();
  typedef OSStatus (*LSSetApplicationInformationItemType)(int, PrivateLSASN,
                                                          CFStringRef,
                                                          CFStringRef,
                                                          CFDictionaryRef *);

  static LSGetCurrentApplicationASNType ls_get_current_application_asn_func =
      NULL;
  static LSSetApplicationInformationItemType
      ls_set_application_information_item_func = NULL;
  static CFStringRef ls_display_name_key = NULL;
  if (!symbol_lookup_status)
  {
    CFBundleRef launch_services_bundle =
        CFBundleGetBundleWithIdentifier(CFSTR("com.apple.LaunchServices"));
    if (!launch_services_bundle)
    {
      symbol_lookup_status = 2;
      return;
    }

    ls_get_current_application_asn_func =
        reinterpret_cast<LSGetCurrentApplicationASNType>(
            CFBundleGetFunctionPointerForName(
                launch_services_bundle, CFSTR("_LSGetCurrentApplicationASN")));
    if (!ls_get_current_application_asn_func)
    {
      symbol_lookup_status = 2;
      return;
    }

    ls_set_application_information_item_func =
        reinterpret_cast<LSSetApplicationInformationItemType>(
            CFBundleGetFunctionPointerForName(
                launch_services_bundle,
                CFSTR("_LSSetApplicationInformationItem")));
    if (!ls_set_application_information_item_func)
    {
      symbol_lookup_status = 2;
      return;
    }

    const CFStringRef *key_pointer = reinterpret_cast<const CFStringRef *>(
        CFBundleGetDataPointerForName(launch_services_bundle,
                                      CFSTR("_kLSDisplayNameKey")));
    ls_display_name_key = key_pointer ? *key_pointer : NULL;
    if (!ls_display_name_key)
    {
      symbol_lookup_status = 2;
      return;
    }

    // Internally, this call relies on the Mach ports that are started up by the
    // Carbon Process Manager.  In debug builds this usually happens due to how
    // the logging layers are started up; but in release, it isn't started in as
    // much of a defined order.  So if the symbols had to be loaded, go ahead
    // and force a call to make sure the manager has been initialized and hence
    // the ports are opened.
    ProcessSerialNumber psn;
    GetCurrentProcess(&psn);
    symbol_lookup_status = 1; // 1=ok
  }

  PrivateLSASN asn = ls_get_current_application_asn_func();
  // Constant used by WebKit; what exactly it means is unknown.
  const int magic_session_constant = -2;
  CFStringRef process_name =
      CFStringCreateWithCString(NULL, title, kCFStringEncodingUTF8);
  OSErr err =
      ls_set_application_information_item_func(magic_session_constant, asn,
                                               ls_display_name_key,
                                               process_name,
                                               NULL /* optional out param */);
}
#endif

void SET_PROCESS_NAME(const std::string &name)
{
#ifdef __ANDROID__
  prctl(PR_SET_NAME, name.c_str());
#elif __APPLE__
  _SetProcessTitleOnDarwin(name);
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

  if (signal == SIGSEGV || signal == SIGBUS || signal == SIGFPE || signal == SIGABRT)
    return exit(signal);

#ifndef _WIN32
  if (signal == SIGQUIT)
    return exit(0);
#endif
}

constexpr int SIGNALS[] = {
    SIGABRT,
    SIGSEGV,
#ifndef _WIN32
    SIGHUP,
    SIGQUIT,
    SIGBUS,
    SIGSYS,
#endif
};

void ENABLE_BACKTRACE()
{
  for (int id : SIGNALS)
    signal(id, printsStacktraceOnSignal);
}
