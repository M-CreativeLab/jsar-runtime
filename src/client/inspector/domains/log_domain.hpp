#pragma once

#include <vector>
#include <string>
#include <client/inspector/content_cdp_handler.hpp>

namespace client_inspector::domains
{
  // Log entry structure based on CDP Log domain specification
  struct LogEntry
  {
    std::string source;            // "xml", "javascript", "network", "storage", "appcache", "rendering", "security", "deprecation", "worker", "violation", "intervention", "recommendation", "other"
    std::string level;             // "verbose", "info", "warning", "error"
    std::string text;              // Log message text
    std::string category;          // Optional log category
    double timestamp;              // Timestamp when the log entry was created
    std::string url;               // Optional URL associated with the log entry
    int lineNumber = 0;            // Optional line number in the resource
    int stackTrace = 0;            // Optional stack trace information
    int networkRequestId = 0;      // Optional network request ID
    int workerId = 0;              // Optional worker ID
    std::vector<std::string> args; // Optional additional arguments
  };

  // Content-side Log domain implementation
  class CdpLogDomain : public ContentCdpDomainHandler
  {
  public:
    CdpLogDomain();
    ~CdpLogDomain() override = default;

    // ContentCdpDomainHandler interface
    std::string handleMethod(const std::string &method, const CdpMessage &message) override;
    std::string getDomainName() const override
    {
      return "Log";
    }
    std::string getDomainDescription() const override
    {
      return "Content-side Log domain for collecting log entries";
    }

    // Add a log entry (can be called from other parts of the content process)
    void addLogEntry(const LogEntry &entry);

  private:
    // Method handlers
    std::string enable(const CdpMessage &message);
    std::string disable(const CdpMessage &message);
    std::string clear(const CdpMessage &message);
    std::string startViolationsReport(const CdpMessage &message);
    std::string stopViolationsReport(const CdpMessage &message);

    // Helper methods
    std::string createLogEntryEvent(const LogEntry &entry);
    rapidjson::Value logEntryToJson(const LogEntry &entry, rapidjson::Document::AllocatorType &allocator);

    bool enabled_ = false;
    bool violationsReportEnabled_ = false;
    std::vector<LogEntry> logBuffer_;
    std::vector<std::string> violationTypes_;
  };
}
