#include <stdarg.h>
#include <cstring>
#include <cxxabi.h>
#include <ctime>
#include <chrono>
#include <string>
#include <memory>
#include <vector>
#include <regex>
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
#include <CoreText/CoreText.h>
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

using namespace std;
using system_clock = std::chrono::system_clock;
using milliseconds = std::chrono::milliseconds;

inline string fetchTimestamp()
{
  using namespace std::chrono;

  auto now = system_clock::now();
  auto now_c = system_clock::to_time_t(now);
  auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

  struct tm tm_info;
  localtime_r(&now_c, &tm_info);

  char timestamp[32];
  strftime(timestamp, sizeof(timestamp), "%m-%d %H:%M:%S", &tm_info);
  snprintf(timestamp + 14, sizeof(timestamp) - 14, ".%03d", static_cast<int>(ms.count()));
  return string(timestamp);
}

namespace stylized_shell
{
  enum class Style : uint32_t
  {
    kDefault = 0,
    kBold = 1,
    kUnderline = 4,
    kReverseColors = 7,
  };

  enum class TextColor : uint32_t
  {
    kBlack = 30,
    kRed = 31,
    kGreen = 32,
    kYellow = 33,
    kBlue = 34,
    kMagenta = 35,
    kCyan = 36,
    kWhite = 37,
  };

  enum class BackgroundColor : uint32_t
  {
    kBlack = 40,
    kRed = 41,
    kGreen = 42,
    kYellow = 43,
    kBlue = 44,
    kMagenta = 45,
    kCyan = 46,
    kWhite = 47,
  };

  inline void start(FILE *stream, bool enabled, Style style, TextColor text)
  {
    if (enabled)
      fprintf(stream, "\033[%d;%dm", static_cast<uint32_t>(style), static_cast<uint32_t>(text));
  }

  inline void end(FILE *stream, bool enabled)
  {
    if (enabled)
      fprintf(stream, "\033[0m");
  }
}

void DEBUG(const char *tag, const char *format, ...)
{
  va_list args;

#ifdef __ANDROID__
  va_start(args, format);
  __android_log_vprint(ANDROID_LOG_DEBUG, tag, format, args);
  va_end(args);
#else
  va_start(args, format);
  int out_len = vsnprintf(nullptr, 0, format, args);
  va_end(args);

  if (out_len < 0) [[unlikely]]
    return;

  vector<char> out_buf(out_len + 1);
  va_start(args, format);
  vsnprintf(out_buf.data(), out_buf.size(), format, args);
  va_end(args);

  FILE *stream = stdout;
  bool stylized = false;
  if (strcmp(tag, LOG_TAG_ERROR) == 0)
  {
    stream = stderr;
    stylized = true;
  }

  stylized_shell::start(stream,
                        stylized,
                        stylized_shell::Style::kDefault,
                        stylized_shell::TextColor::kRed);
  {
    string timestamp = fetchTimestamp();
    const char *p = out_buf.data();
    bool is_new_line = true;

    while (*p != '\0')
    {
      if (is_new_line)
      {
        fprintf(stream, "%s ", timestamp.c_str());
        is_new_line = false;
      }

      if (*p == '\n')
      {
        fputc('\n', stream);
        is_new_line = true;
      }
      else
      {
        fputc(*p, stream);
      }
      p++;
    }

    if (!is_new_line)
      fputc('\n', stream);
  }
  stylized_shell::end(stream, stylized);
#endif
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
void _SetProcessTitleOnDarwin(const std::string &title)
{
  // TODO(yorkie): implement this.
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

// Demangle the symbol name and return the demangled name.
std::string _DemangleSymbol(const std::string &symbol)
{
  int status;
  std::unique_ptr<char, void (*)(void *)> result(
    abi::__cxa_demangle(symbol.c_str(), nullptr, nullptr, &status),
    std::free);
  return (status == 0) ? result.get() : symbol;
}

// Process the symbol line which replaces the mangled symbol with demangled symbol.
std::string _ProcessSymbolLine(const char *line)
{
  static std::regex regex(R"(\b(_Z[\w\d]+)\b)"); // Search for mangled symbols

  std::string result = line;
  std::sregex_iterator it(result.begin(), result.end(), regex);
  std::sregex_iterator end;

  while (it != end)
  {
    std::smatch match = *it;
    std::string mangled = match[1].str();
    std::string demangled = _DemangleSymbol(mangled);
    size_t pos = match.position(1);
    result.replace(pos, mangled.length(), demangled);
    it = std::sregex_iterator(result.begin() + pos + demangled.length(), result.end(), regex);
  }
  return result;
}

void _PrintsStacktraceOnSignal(int signal)
{
  std::cerr << "===================== Transmute Crash Report ===================" << std::endl;
  std::cerr << "Received SIGNAL(" << signal << ")" << std::endl;

#ifdef __APPLE__
  const int maxFrames = 20;
  void *stackTrace[maxFrames];
  int numFrames = backtrace(stackTrace, maxFrames);
  char **symbols = backtrace_symbols(stackTrace, numFrames);
  if (symbols == nullptr)
  {
    fprintf(stderr, "[darwin]: Failed to obtain backtrace symbols");
  }
  else
  {
    for (int i = 0; i < numFrames; ++i)
      fprintf(stderr, "%s\n", _ProcessSymbolLine(symbols[i]).c_str());
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
    std::cerr << "[android]: failed to unwind backtrace symbols." << std::endl;
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
        fprintf(stdout, "  #%d pc %p %s", n, addr, status == 0 && demangled != nullptr ? demangled : symbol);

        if (demangled != nullptr)
          free(demangled);
      }
    }
  }
#endif

  if (signal == SIGSEGV || signal == SIGBUS || signal == SIGFPE || signal == SIGABRT)
  {
    std::cerr << "Exiting(1) due to signal(" << signal << ")" << std::endl;
    return exit(1);
  }

#ifndef _WIN32
  if (signal == SIGQUIT)
  {
    std::cerr << "Exiting(0) due to signal(" << signal << ")" << std::endl;
    return exit(0);
  }
#endif
}

// Signals to be watched
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
  /**
   * `TR_ENABLE_SIGNALS_ONSTACK` is to enable signal handling on a new alternate stack, which is useful
   * when the stack is corrupted such as stack overflow or underflow.
   *
   * If you met the backtrace not printing, you can try to enable this macro.
   */
#ifdef TR_ENABLE_SIGNALS_ONSTACK
#define STACK_SIZE (SIGSTKSZ)
  stack_t ss;
  struct sigaction sa;

  ss.ss_sp = malloc(STACK_SIZE);
  if (ss.ss_sp == NULL)
  {
    perror("malloc");
    return exit(1);
  }
  ss.ss_size = STACK_SIZE;
  ss.ss_flags = 0;

  if (sigaltstack(&ss, NULL) == -1)
  {
    perror("sigaltstack");
    return exit(1);
  }

  sa.sa_flags = SA_ONSTACK;
#ifdef __APPLE__
  sa.__sigaction_u.__sa_handler = _PrintsStacktraceOnSignal;
#else
  sa.sa_handler = _PrintsStacktraceOnSignal;
#endif
  sigemptyset(&sa.sa_mask);

  for (int id : SIGNALS)
  {
    if (sigaction(id, &sa, NULL) == -1)
      DEBUG(LOG_TAG_ERROR, "Failed to set signal action for %d", id);
  }
#else
  for (int id : SIGNALS)
    signal(id, _PrintsStacktraceOnSignal);
#endif
}
