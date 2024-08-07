#pragma once

#include <string>
#include <stdint.h>

using namespace std;

#define TR_DOCUMENT_EVENTS_MAP(XX)       \
  XX(SpawnProcess, spawnprocess)         \
  XX(BeforeScripting, beforescripting)   \
  XX(BeforeLoading, beforeloading)       \
  XX(DispatchRequest, dispatchrequest)   \
  XX(Loaded, loaded)                     \
  XX(Load, load)                         \
  XX(DOMContentLoaded, DOMContentLoaded) \
  XX(FCP, fcp)                           \
  XX(LCP, lcp)                           \
  XX(Error, error)

/**
 * The type of the document event which could be marked as the lifecycle event or the metrics event.
 *
 * Lifecycle Events:
 * - SpawnProcess: The event is triggered when the content process is spawned.
 * - BeforeScripting: The event is triggered before the document starts scripting.
 * - BeforeLoading: The event is triggered before the document starts loading.
 * - DispatchRequest: The event is triggered when the document dispatches a request.
 * - Loaded: The event is triggered when the document is loaded.
 * - Load: The event is triggered when the document is loaded.
 * - DOMContentLoaded: The event is triggered when the document is parsed and the DOM tree is built.
 * - Error: The event is triggered when an error occurs.
 *
 * Metrics Events:
 * - FCP: The event is triggered when the first contentful paint occurs.
 * - LCP: The event is triggered when the largest contentful paint occurs.
 */
enum class TrDocumentEventType
{
  Unknown = 0x10,
#define XX(eventType, _) eventType,
  TR_DOCUMENT_EVENTS_MAP(XX)
#undef XX
};

enum class TrScriptRunMode
{
  Dangerously,
  Safely,
  Disabled
};

class TrDocumentRequestInit
{
public:
  TrDocumentRequestInit() = default;
  TrDocumentRequestInit(TrDocumentRequestInit &that) = default;

public:
  string getRunScriptsName()
  {
    switch (runScripts)
    {
    case TrScriptRunMode::Dangerously:
      return "dangerously";
    case TrScriptRunMode::Safely:
      return "safely";
    case TrScriptRunMode::Disabled:
      return "disabled";
    }
    return "dangerously";
  }

public:
  string url;
  uint32_t id = 0;
  bool disableCache = false;
  bool isPreview = false;
  TrScriptRunMode runScripts = TrScriptRunMode::Dangerously;
};

inline string documentEventToName(TrDocumentEventType type)
{
  switch (type)
  {
  case TrDocumentEventType::SpawnProcess:
    return "spawnprocess";
  case TrDocumentEventType::BeforeScripting:
    return "beforescripting";
  case TrDocumentEventType::BeforeLoading:
    return "beforeloading";
  case TrDocumentEventType::DispatchRequest:
    return "dispatchrequest";
  case TrDocumentEventType::Loaded:
    return "loaded";
  case TrDocumentEventType::Load:
    return "load";
  case TrDocumentEventType::DOMContentLoaded:
    return "DOMContentLoaded";
  case TrDocumentEventType::FCP:
    return "fcp";
  case TrDocumentEventType::LCP:
    return "lcp";
  case TrDocumentEventType::Error:
    return "error";
  default:
    return "unknown";
  }
}
