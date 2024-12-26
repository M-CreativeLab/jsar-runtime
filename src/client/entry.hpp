#pragma once

#include <string>
#include "./per_process.hpp"

/**
 * The client could be in 2 modes: Hive or Client.
 */
enum class TrClientMode
{
  Hive,
  Client,
};

/**
 * The entry point for the client.
 */
class TrClientEntry final
{
public:
  /**
   * Constructor.
   * 
   * @param mode The mode of the client.
   */
  TrClientEntry(TrClientMode mode);
  ~TrClientEntry() = default;

public:
  /**
   * Run the entry point from a given configuration and URL.
   */
  int run(std::string configJson, std::string url);

private:
  /**
   * Parse the configuration JSON into the client context.
   * 
   * @param configJson The configuration JSON.
   */
  bool parseConfig(std::string &configJson);
  /**
   * When the client is in Hive mode.
   */
  int onHiveMode();
  /**
   * When the client is in Client mode.
   */
  int onClientMode(TrDocumentRequestInit &init);

public:
  TrClientMode mode;
  TrClientContextPerProcess *clientContext = nullptr;

private:
  int hivePort = -1;

  friend class TrHiveServer;
};
