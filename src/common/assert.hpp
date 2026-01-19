#pragma once

namespace transmute::common
{
  void BreakPoint();
  void HandleAssertionFailure(const char *file,
                              const char *function,
                              int line,
                              const char *condition);
}
