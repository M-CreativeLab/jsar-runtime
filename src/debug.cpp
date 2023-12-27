#include <stdarg.h>
#include "debug.hpp"
#include "logger.hpp"

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
  TransmuteLogger::GetInstance()->writeStdout(buffer);
#endif

  delete[] buffer;
  va_end(args);
}
