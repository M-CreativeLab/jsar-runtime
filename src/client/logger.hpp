#pragma once

#include <string>
#include <memory>
#include "inspector/content_log_domain.hpp"

// Forward declarations
class TrClientContextPerProcess;
class ContentCdpHandler;

/**
 * Client-side logger that sends log messages to DevTools via CDP Log domain
 * Singleton pattern for global access
 */
class Logger
{
public:
  // Log levels matching CDP Log.LogEntry
  enum class Level
  {
    VERBOSE,
    INFO,
    WARNING,
    ERROR
  };

  // Log sources matching CDP Log.LogEntry
  enum class Source
  {
    XML,
    JAVASCRIPT,
    NETWORK,
    CONSOLE_API,
    STORAGE,
    APPCACHE,
    RENDERING,
    SECURITY,
    DEPRECATION,
    WORKER,
    VIOLATION,
    INTERVENTION,
    RECOMMENDATION,
    OTHER
  };

  // Singleton access
  static Logger *getInstance();

  // Set the current client context for CDP integration
  void setClientContext(TrClientContextPerProcess *clientContext);

  // Main logging methods
  void log(Level level, Source source, const std::string &text);
  void log(Level level, Source source, const std::string &text, const std::string &url, int lineNumber = 0);

  // Convenience methods
  void verbose(const std::string &text);
  void info(const std::string &text);
  void warning(const std::string &text);
  void error(const std::string &text);

  // JavaScript-specific logging
  void jsLog(const std::string &text);
  void jsInfo(const std::string &text);
  void jsWarn(const std::string &text);
  void jsError(const std::string &text);

private:
  Logger() = default;
  ~Logger() = default;
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  TrClientContextPerProcess *clientContext_ = nullptr;

  // Helper methods
  std::string levelToString(Level level);
  std::string sourceToString(Source source);
  ContentCdpHandler *getContentCdpHandler();
};