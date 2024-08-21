#include <rapidjson/document.h>
#include <semaphore.h>
#include "common/debug.hpp"
#include "./entry.hpp"
#include "./hive_server.hpp"

TrClientEntry::TrClientEntry(TrClientMode mode) : mode(mode)
{
  clientContext = TrClientContextPerProcess::Create();
}

int TrClientEntry::run(string configJson, string url)
{
  if (!parseConfig(configJson))
    return 1;

  clientContext->preload();
  if (mode == TrClientMode::Hive)
  {
    return onHiveMode();
  }
  else
  {
    TrDocumentRequestInit init;
    init.url = url;
    return onClientMode(init);
  }
}

bool TrClientEntry::parseConfig(string &configJson)
{
  rapidjson::Document document;
  document.Parse(configJson.c_str());

  if (document.HasParseError())
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "Exited, reason: failed to parse the context json \"%s\"", configJson.c_str());
    return false;
  }
  if (!document.HasMember("eventChanPort") || !document["eventChanPort"].IsUint())
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "Exited, reason: `eventChanPort` is missing or not a number from context json.");
    return false;
  }
  if (!document.HasMember("frameChanPort") || !document["frameChanPort"].IsUint())
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "Exited, reason: `frameChanPort` is missing or not a number from context json.");
    return false;
  }
  if (!document.HasMember("commandBufferChanPort") || !document["commandBufferChanPort"].IsUint())
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "Exited, reason: `commandBufferChanPort` is missing or not a number from context json.");
    return false;
  }

  // Update hive port if it's available.
  if (document.HasMember("hiveChanPort") && document["hiveChanPort"].IsUint())
    hivePort = document["hiveChanPort"].GetUint();

  // Ports
  clientContext->eventChanPort = document["eventChanPort"].GetUint();
  clientContext->frameChanPort = document["frameChanPort"].GetUint();
  clientContext->mediaChanPort = document["mediaChanPort"].GetUint();
  clientContext->commandBufferChanPort = document["commandBufferChanPort"].GetUint();

  // Global settings
  if (document.HasMember("applicationCacheDirectory"))
    clientContext->applicationCacheDirectory = document["applicationCacheDirectory"].GetString();
  if (document.HasMember("httpsProxyServer"))
    clientContext->httpsProxyServer = document["httpsProxyServer"].GetString();
  if (document.HasMember("enableV8Profiling") && document["enableV8Profiling"].IsBool())
    clientContext->enableV8Profiling = document["enableV8Profiling"].GetBool();

  // XR Device settings
  if (document.HasMember("xrDevice") && document["xrDevice"].IsObject())
  {
    auto &xrDeviceDoc = document["xrDevice"];
    if (xrDeviceDoc.HasMember("enabled") && xrDeviceDoc["enabled"].IsBool())
      clientContext->xrDeviceInit.enabled = xrDeviceDoc["enabled"].GetBool();
    if (xrDeviceDoc.HasMember("active") && xrDeviceDoc["active"].IsBool())
      clientContext->xrDeviceInit.active = xrDeviceDoc["active"].GetBool();
    if (xrDeviceDoc.HasMember("stereoRenderingMode") && xrDeviceDoc["stereoRenderingMode"].IsNumber())
      clientContext->xrDeviceInit.stereoRenderingMode = (xr::TrStereoRenderingMode)xrDeviceDoc["stereoRenderingMode"].GetInt();
    if (xrDeviceDoc.HasMember("commandChanPort") && xrDeviceDoc["commandChanPort"].IsInt())
      clientContext->xrDeviceInit.commandChanPort = xrDeviceDoc["commandChanPort"].GetInt();
    if (xrDeviceDoc.HasMember("sessionContextZoneDirectory") && xrDeviceDoc["sessionContextZoneDirectory"].IsString())
      clientContext->xrDeviceInit.sessionContextZoneDirectory = xrDeviceDoc["sessionContextZoneDirectory"].GetString();
    if (xrDeviceDoc.HasMember("deviceContextZonePath") && xrDeviceDoc["deviceContextZonePath"].IsString())
      clientContext->xrDeviceInit.deviceContextZonePath = xrDeviceDoc["deviceContextZonePath"].GetString();
    if (xrDeviceDoc.HasMember("inputSourcesZonePath") && xrDeviceDoc["inputSourcesZonePath"].IsString())
      clientContext->xrDeviceInit.inputSourcesZonePath = xrDeviceDoc["inputSourcesZonePath"].GetString();
  }
  else
  {
    fprintf(stderr, "There is no \"xrDevice\" field from the init JSON.\n");
  }
  return true;
}

int TrClientEntry::onHiveMode()
{
  SET_PROCESS_NAME("jsar_hive");
  TrHiveServer server(this, hivePort);
  server.start();

  if (server.isChild)
    return onClientMode(server.requestInit);
  else
    return 0;
}

int TrClientEntry::onClientMode(TrDocumentRequestInit &init)
{
  string processTitle = "jsar_app(" + std::to_string(init.id) + ") " + init.url;
  SET_PROCESS_NAME(processTitle);

  clientContext->id = init.id;
  clientContext->url = init.url;
  clientContext->print();
  clientContext->start();

  TrScriptRuntimePerProcess runtime;
  vector<string> args = {
      "--url",
      clientContext->url,
      "--id",
      std::to_string(clientContext->id),
  };
  runtime.start(args);
  fprintf(stdout, "The client(%d|%s) is stopped.\n", clientContext->id, clientContext->url.c_str());
  return 0;
}
