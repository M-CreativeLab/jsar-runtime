#include <iostream>
#include <sstream>
#include <common/assert.hpp>
#include <common/utility.hpp>
#include <common/command_buffers/gpu/error.hpp>
#include <common/command_buffers/gpu/error_data.hpp>

using namespace std;

namespace commandbuffers::gpu
{
  unique_ptr<ErrorData> ErrorData::Create(InternalErrorType type,
                                          string message,
                                          const char *file,
                                          const char *function,
                                          int line)
  {
    unique_ptr<ErrorData> error = make_unique<ErrorData>(type, move(message));
    error->appendBacktrace(file, function, line);

    auto [var, present] = transmute::common::GetEnvironmentVar("DAWN_DEBUG_BREAK_ON_ERROR");
    if (present && !var.empty() && var != "0")
    {
      cerr << error->message() << endl;
      transmute::common::BreakPoint();
    }
    return error;
  }

  ErrorData::ErrorData(InternalErrorType type, string message)
      : type_(type)
      , message_(move(message))
  {
  }

  void ErrorData::appendBacktrace(const char *file, const char *function, int line)
  {
    BacktraceRecord record;
    record.file = file;
    record.function = function;
    record.line = line;

    backtrace_.push_back(move(record));
  }

  void ErrorData::appendContext(string context)
  {
    contexts_.push_back(move(context));
  }

  void ErrorData::appendDebugGroup(string_view label)
  {
    debug_groups_.push_back(string(label));
  }

  void ErrorData::appendBackendMessage(string message)
  {
    backend_messages_.push_back(move(message));
  }

  InternalErrorType ErrorData::type() const
  {
    return type_;
  }

  const string &ErrorData::message() const
  {
    return message_;
  }

  const vector<ErrorData::BacktraceRecord> &ErrorData::backtrace() const
  {
    return backtrace_;
  }

  const vector<string> &ErrorData::contexts() const
  {
    return contexts_;
  }

  const vector<string> &ErrorData::debugGroups() const
  {
    return debug_groups_;
  }

  const vector<string> &ErrorData::backendMessages() const
  {
    return backend_messages_;
  }

  string ErrorData::getFormattedMessage() const
  {
    ostringstream ss;
    ss << message_ << "\n";

    if (!contexts_.empty())
    {
      for (auto context : contexts_)
      {
        ss << " - While " << context << "\n";
      }
    }

    // For non-validation errors, or errors that lack a context include the
    // stack trace for debugging purposes.
    if (contexts_.empty() || type_ != InternalErrorType::kValidation)
    {
      for (const auto &callsite : backtrace_)
      {
        ss << "    at " << callsite.function << " (" << callsite.file << ":" << callsite.line
           << ")\n";
      }
    }

    if (!debug_groups_.empty())
    {
      ss << "\nDebug group stack:\n";
      for (auto label : debug_groups_)
      {
        ss << " > \"" << label << "\"\n";
      }
    }

    if (!backend_messages_.empty())
    {
      ss << "\nBackend messages:\n";
      for (auto message : backend_messages_)
      {
        ss << " * " << message << "\n";
      }
    }

    return ss.str();
  }
}