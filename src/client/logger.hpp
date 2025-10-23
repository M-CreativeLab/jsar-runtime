#pragma once

#include <string>
#include <memory>

// Forward declarations
class TrClientContextPerProcess;

namespace endor
{
  namespace client_inspector
  {
    class ContentCdpHandler;
  }

  namespace logging
  {

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
      static Logger *GetInstance();

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
      client_inspector::ContentCdpHandler *getContentCdpHandler();
    };

    // Convenience functions for global access
    void LogVerbose(const std::string &text);
    void LogInfo(const std::string &text);
    void LogWarning(const std::string &text);
    void LogError(const std::string &text);

    // JavaScript-specific convenience functions
    void LogJsInfo(const std::string &text);
    void LogJsWarn(const std::string &text);
    void LogJsError(const std::string &text);

    // General logging with level and source
    void Log(Logger::Level level, Logger::Source source, const std::string &text);
    void Log(Logger::Level level, Logger::Source source, const std::string &text, const std::string &url, int lineNumber = 0);

  } // namespace logging
} // namespace endor