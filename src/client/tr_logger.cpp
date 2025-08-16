#include "tr_logger.hpp"
#include "per_process.hpp"
#include "inspector/content_cdp_handler.hpp"
#include "inspector/content_log_domain.hpp"
#include "debug.hpp"

TrLogger::TrLogger(TrClientContextPerProcess *clientContext)
    : clientContext_(clientContext)
{
}

TrLogger::~TrLogger()
{
}

void TrLogger::log(Level level, Source source, const std::string &text)
{
  log(level, source, text, "", 0);
}

void TrLogger::log(Level level, Source source, const std::string &text, const std::string &url, int lineNumber)
{
#ifdef TR_ENABLE_INSPECTOR
  auto cdpHandler = getContentCdpHandler();
  if (!cdpHandler)
  {
    DEBUG(LOG_TAG_INSPECTOR, "TrLogger: No CDP handler available, skipping log entry");
    return;
  }

  auto logDomain = cdpHandler->getLogDomain();
  if (!logDomain)
  {
    DEBUG(LOG_TAG_INSPECTOR, "TrLogger: No Log domain available, skipping log entry");
    return;
  }

  // Create log entry
  LogEntry entry;
  entry.source = sourceToString(source);
  entry.level = levelToString(level);
  entry.text = text;
  entry.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
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

  DEBUG(LOG_TAG_INSPECTOR, "TrLogger: Sent log entry to DevTools: [%s/%s] %s", entry.level.c_str(), entry.source.c_str(), text.c_str());
#else
  // Fallback to regular debug logging when inspector is disabled
  DEBUG(LOG_TAG_JSAR, "TrLogger: %s", text.c_str());
#endif
}

void TrLogger::verbose(const std::string &text)
{
  log(Level::VERBOSE, Source::OTHER, text);
}

void TrLogger::info(const std::string &text)
{
  log(Level::INFO, Source::OTHER, text);
}

void TrLogger::warning(const std::string &text)
{
  log(Level::WARNING, Source::OTHER, text);
}

void TrLogger::error(const std::string &text)
{
  log(Level::ERROR, Source::OTHER, text);
}

void TrLogger::jsLog(const std::string &text)
{
  log(Level::INFO, Source::JAVASCRIPT, text);
}

void TrLogger::jsInfo(const std::string &text)
{
  log(Level::INFO, Source::JAVASCRIPT, text);
}

void TrLogger::jsWarn(const std::string &text)
{
  log(Level::WARNING, Source::JAVASCRIPT, text);
}

void TrLogger::jsError(const std::string &text)
{
  log(Level::ERROR, Source::JAVASCRIPT, text);
}

std::string TrLogger::levelToString(Level level)
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

std::string TrLogger::sourceToString(Source source)
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

ContentCdpHandler *TrLogger::getContentCdpHandler()
{
#ifdef TR_ENABLE_INSPECTOR
  if (clientContext_)
  {
    return clientContext_->getContentCdpHandler();
  }
#endif
  return nullptr;
}