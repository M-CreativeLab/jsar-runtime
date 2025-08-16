#include <iostream>

#include "./inspector/content_cdp_handler.hpp"
#include "./inspector/content_log_domain.hpp"
#include "./per_process.hpp"
#include "./logger.hpp"
#include "./debug.hpp"

namespace logging
{
  using namespace std;

  Logger *Logger::GetInstance()
  {
    static Logger instance;
    return &instance;
  }

  void Logger::setClientContext(TrClientContextPerProcess *clientContext)
  {
    clientContext_ = clientContext;
  }

  void Logger::log(Level level, Source source, const string &text)
  {
    log(level, source, text, "", 0);
  }

  void Logger::log(Level level, Source source, const string &text, const string &url, int lineNumber)
  {
    if (level == Level::ERROR || level == Level::WARNING)
      cerr << "[" << sourceToString(source) << "] " << text << endl;
    else
      cout << "[" << sourceToString(source) << "] " << text << endl;

#ifdef TR_ENABLE_INSPECTOR
    auto cdpHandler = getContentCdpHandler();
    if (!cdpHandler)
    {
      DEBUG(LOG_TAG_INSPECTOR, "Logger: No CDP handler available, skipping log entry");
      // Fallback to standard error when CDP is not available
      cerr << "Logger: " << text << endl;
      return;
    }

    auto logDomain = cdpHandler->getLogDomain();
    if (!logDomain)
    {
      DEBUG(LOG_TAG_INSPECTOR, "Logger: No Log domain available, skipping log entry");
      // Fallback to standard error when Log domain is not available
      cerr << "Logger: " << text << endl;
      return;
    }

    // Create log entry
    LogEntry entry;
    entry.source = sourceToString(source);
    entry.level = levelToString(level);
    entry.text = text;
    entry.timestamp = chrono::duration_cast<chrono::milliseconds>(
                        chrono::system_clock::now().time_since_epoch())
                        .count();

    if (!url.empty())
    {
      entry.url = url;
    }

    if (lineNumber > 0)
    {
      entry.lineNumber = lineNumber;
    }

    // Send to CDP Log domain
    logDomain->addLogEntry(entry);
#endif
  }

  void Logger::verbose(const string &text)
  {
    log(Level::VERBOSE, Source::OTHER, text);
  }

  void Logger::info(const string &text)
  {
    log(Level::INFO, Source::OTHER, text);
  }

  void Logger::warning(const string &text)
  {
    log(Level::WARNING, Source::OTHER, text);
  }

  void Logger::error(const string &text)
  {
    log(Level::ERROR, Source::OTHER, text);
  }

  void Logger::jsLog(const string &text)
  {
    log(Level::INFO, Source::JAVASCRIPT, text);
  }

  void Logger::jsInfo(const string &text)
  {
    log(Level::INFO, Source::JAVASCRIPT, text);
  }

  void Logger::jsWarn(const string &text)
  {
    log(Level::WARNING, Source::JAVASCRIPT, text);
  }

  void Logger::jsError(const string &text)
  {
    log(Level::ERROR, Source::JAVASCRIPT, text);
  }

  string Logger::levelToString(Level level)
  {
    switch (level)
    {
    case Level::VERBOSE:
      return "verbose";
    case Level::INFO:
      return "info";
    case Level::WARNING:
      return "warning";
    case Level::ERROR:
      return "error";
    default:
      return "info";
    }
  }

  string Logger::sourceToString(Source source)
  {
    switch (source)
    {
    case Source::XML:
      return "xml";
    case Source::JAVASCRIPT:
      return "javascript";
    case Source::NETWORK:
      return "network";
    case Source::CONSOLE_API:
      return "console-api";
    case Source::STORAGE:
      return "storage";
    case Source::APPCACHE:
      return "appcache";
    case Source::RENDERING:
      return "rendering";
    case Source::SECURITY:
      return "security";
    case Source::DEPRECATION:
      return "deprecation";
    case Source::WORKER:
      return "worker";
    case Source::VIOLATION:
      return "violation";
    case Source::INTERVENTION:
      return "intervention";
    case Source::RECOMMENDATION:
      return "recommendation";
    case Source::OTHER:
      return "other";
    default:
      return "other";
    }
  }

  ContentCdpHandler *Logger::getContentCdpHandler()
  {
#ifdef TR_ENABLE_INSPECTOR
    if (clientContext_)
    {
      return clientContext_->getContentCdpHandler();
    }
#endif
    return nullptr;
  }

  // Convenience functions for global access
  void LogVerbose(const string &text)
  {
    Logger::GetInstance()->verbose(text);
  }

  void LogInfo(const string &text)
  {
    Logger::GetInstance()->info(text);
  }

  void LogWarning(const string &text)
  {
    Logger::GetInstance()->warning(text);
  }

  void LogError(const string &text)
  {
    Logger::GetInstance()->error(text);
  }

  void LogJsInfo(const string &text)
  {
    Logger::GetInstance()->jsInfo(text);
  }

  void LogJsWarn(const string &text)
  {
    Logger::GetInstance()->jsWarn(text);
  }

  void LogJsError(const string &text)
  {
    Logger::GetInstance()->jsError(text);
  }

  void Log(Logger::Level level, Logger::Source source, const string &text)
  {
    Logger::GetInstance()->log(level, source, text);
  }

  void Log(Logger::Level level, Logger::Source source, const string &text, const string &url, int lineNumber)
  {
    Logger::GetInstance()->log(level, source, text, url, lineNumber);
  }

} // namespace logging