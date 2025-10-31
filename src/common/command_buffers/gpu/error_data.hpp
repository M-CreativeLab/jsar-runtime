#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace commandbuffers::gpu
{
  enum class InternalErrorType : uint32_t
  {
    kNone = 0,
    kValidation = 1,
    kDeviceLost = 2,
    kInternal = 4,
    kOutOfMemory = 8
  };

  class [[nodiscard]] ErrorData
  {
  public:
    [[nodiscard]] static std::unique_ptr<ErrorData> Create(InternalErrorType type,
                                                           std::string message,
                                                           const char *file,
                                                           const char *function,
                                                           int line);
    ErrorData(InternalErrorType type, std::string message);

    struct BacktraceRecord
    {
      const char *file;
      const char *function;
      int line;
    };
    void appendBacktrace(const char *file, const char *function, int line);
    void appendContext(std::string context);
    void appendDebugGroup(std::string_view label);
    void appendBackendMessage(std::string message);

    InternalErrorType type() const;
    const std::string &message() const;
    const std::vector<BacktraceRecord> &backtrace() const;
    const std::vector<std::string> &contexts() const;
    const std::vector<std::string> &debugGroups() const;
    const std::vector<std::string> &backendMessages() const;

    std::string getFormattedMessage() const;

  private:
    InternalErrorType type_;
    std::string message_;
    std::vector<BacktraceRecord> backtrace_;
    std::vector<std::string> contexts_;
    std::vector<std::string> debug_groups_;
    std::vector<std::string> backend_messages_;
  };
}