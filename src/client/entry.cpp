#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <rapidjson/document.h>

#include "debug.hpp"
#include "per_process.hpp"
#include "common/xr/types.hpp"

using namespace std;

int main(int argc, char **argv)
{
  ENABLE_BACKTRACE();

  for (uint32_t i = 0; i < argc; i++)
    DEBUG(LOG_TAG_CLIENT_ENTRY, "argv[%d] = %s", i, argv[i]);

  if (argc <= 2)
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "Exited, reason: invalid arguments to JSAR client.");
    return 1;
  }
  else
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "Copy the following command to restart the client:");
    DEBUG(LOG_TAG_CLIENT_ENTRY, "  %s '%s' '%s'", argv[0], argv[1], argv[2]);
  }

  string url = string(argv[1]);
  rapidjson::Document contextDocument;
  contextDocument.Parse(argv[2]);

  if (contextDocument.HasParseError())
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "Exited, reason: failed to parse the context json \"%s\"", argv[1]);
    return 1;
  }
  if (!contextDocument.HasMember("eventChanPort") || !contextDocument["eventChanPort"].IsUint())
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "Exited, reason: `eventChanPort` is missing or not a number from context json.");
    return 1;
  }
  if (!contextDocument.HasMember("frameChanPort") || !contextDocument["frameChanPort"].IsUint())
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "Exited, reason: `frameChanPort` is missing or not a number from context json.");
    return 1;
  }
  if (!contextDocument.HasMember("commandBufferChanPort") || !contextDocument["commandBufferChanPort"].IsUint())
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "Exited, reason: `commandBufferChanPort` is missing or not a number from context json.");
    return 1;
  }

  auto clientContext = TrClientContextPerProcess::Create(); // create a new client context globally in child process.
  if (!contextDocument.HasMember("id"))
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "Exited, reason: id is missing from context json.");
    return 1;
  }
  clientContext->id = contextDocument["id"].GetInt();
  clientContext->url = url;
  clientContext->eventChanPort = contextDocument["eventChanPort"].GetUint();
  clientContext->frameChanPort = contextDocument["frameChanPort"].GetUint();
  clientContext->commandBufferChanPort = contextDocument["commandBufferChanPort"].GetUint();

  if (contextDocument.HasMember("applicationCacheDirectory"))
    clientContext->applicationCacheDirectory = contextDocument["applicationCacheDirectory"].GetString();
  if (contextDocument.HasMember("httpsProxyServer"))
    clientContext->httpsProxyServer = contextDocument["httpsProxyServer"].GetString();
  if (contextDocument.HasMember("xrDevice") && contextDocument["xrDevice"].IsObject())
  {
    auto &xrDevice = contextDocument["xrDevice"];
    if (xrDevice.HasMember("enabled") && xrDevice["enabled"].IsBool())
      clientContext->xrDeviceInit.enabled = xrDevice["enabled"].GetBool();
    if (xrDevice.HasMember("active") && xrDevice["active"].IsBool())
      clientContext->xrDeviceInit.active = xrDevice["active"].GetBool();
    if (xrDevice.HasMember("stereoRenderingMode") && xrDevice["stereoRenderingMode"].IsNumber())
      clientContext->xrDeviceInit.stereoRenderingMode = (xr::TrStereoRenderingMode)xrDevice["stereoRenderingMode"].GetInt();
    if (xrDevice.HasMember("commandChanPort") && xrDevice["commandChanPort"].IsInt())
      clientContext->xrDeviceInit.commandChanPort = xrDevice["commandChanPort"].GetInt();
    if (xrDevice.HasMember("inputSourcesZonePath") && xrDevice["inputSourcesZonePath"].IsString())
      clientContext->xrDeviceInit.inputSourcesZonePath = xrDevice["inputSourcesZonePath"].GetString();
  }
  else
  {
    DEBUG(LOG_TAG_CLIENT_ENTRY, "There is no \"xrDevice\" field from the init JSON.");
  }
  clientContext->print(); // prints the client context before the script starts.
  clientContext->start(); // starts the client context.

  TrScriptRuntimePerProcess runtime;
  vector<string> args = {
      "--url",
      url,
      "--id",
      std::to_string(clientContext->id),
  };
  runtime.start(args);

  DEBUG(LOG_TAG_CLIENT_ENTRY, "Client instance is stopped normally.");
  return 0;
}
