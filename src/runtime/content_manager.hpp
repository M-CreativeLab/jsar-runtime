#pragma once

#include "./constellation.hpp"
#include "./content.hpp"

/**
 * A `TrContentManager` is to manage the lifecycle of content instances, that is, to create, run, and dispose of JavaScript runtime
 * environments.
 */
class TrContentManager final
{
  friend class TrContentRuntime;
  friend class TrConstellation;
  friend class TrRenderer;
  friend class TrInspector;

public:
  TrContentManager(TrConstellation *constellation);
  ~TrContentManager();

public:
  bool initialize();
  bool shutdown();
  bool tickOnFrame();
  bool hasContents()
  {
    return !contents.empty();
  }

  /**
   * Set the request authorization headers for upcoming content(client-side) HTTP requests.
   * 
   * @param raw_headers The raw headers string.
   * @param allowed_origins The allowed origins for the raw headers, if empty, all origins are allowed.
   */
  inline void setRequestAuthorizationHeaders(
    const std::string &raw_headers,
    const std::vector<std::string> &allowed_origins = std::vector<std::string>())
  {
    request_authorization_headers.raw_headers = raw_headers;
    request_authorization_headers.allowed_origins = allowed_origins;
  }

  /**
   * Make a new content instance, this doesn't start the content process, just created a `TrContentRuntime` instance and added it
   * to the managed list.
   *
   * @returns The content instance.
   */
  std::shared_ptr<TrContentRuntime> makeContent();
  /**
   * Get the content instance by its id.
   *
   * @param id The content id.
   * @param includePreContent If true, it will return the pre-content instance if the id is matched.
   * @returns The content instance if found, or nullptr if not found.
   */
  std::shared_ptr<TrContentRuntime> getContent(uint32_t id, bool includePreContent = false);
  /**
   * Find the content instance by its client process id.
   *
   * @param pid The client process id.
   * @returns The content instance if found, or nullptr if not found.
   */
  std::shared_ptr<TrContentRuntime> findContentByPid(pid_t pid);
  /**
   * Dispose the content instance, it will release the related resources and terminate the client process.
   *
   * @param content The content instance to dispose.
   */
  void disposeContent(shared_ptr<TrContentRuntime> content);
  /**
   * Dispose all the content instances.
   */
  void disposeAll();

private:
  void onNewClientOnEventChan(TrOneShotClient<events_comm::TrNativeEventMessage> &client);
  void onTryDestroyingContents();
  void onRpcRequest(std::shared_ptr<events_comm::TrNativeEvent> event);
  void onDocumentEvent(std::shared_ptr<events_comm::TrNativeEvent> event);

private:
  /**
   * Install the executable and its dependencies libraries to the runtime directory.
   *
   * This method will install the TransmuteClient executable from the library itself to the runtime directory, and it will also
   * install the dependencies libraries such as `libnode.so`.
   */
  void installExecutable();
  /**
   * Install the bundled JavaScript scripts, which are used to bootstrap the content runtime at client process.
   */
  void installScripts();
  /**
   * Start the hived process, that is, the Hive Daemon, which is the incubator of the content client processes.
   *
   * The `TrHiveDaemon` instance is an agent for the hive daemon process, it provides the methods to create, terminate and messaging
   * with the content processes.
   */
  void startHived();
  void preparePreContent();
  void acceptEventChanClients(int timeout = 100);

private:
  TrConstellation *constellation = nullptr;
  shared_mutex contentsMutex;
  std::vector<std::shared_ptr<TrContentRuntime>> contents;
  std::unique_ptr<TrHiveDaemon> hived;

private: // content listeners
  std::shared_ptr<events_comm::TrNativeEventListener> rpcRequestListener = nullptr;
  std::shared_ptr<events_comm::TrNativeEventListener> documentEventListener = nullptr;

private: // pre-content
  bool enablePreContent = true;
  atomic<bool> preContentScheduled = false;
  chrono::time_point<chrono::system_clock> preContentScheduledTimepoint;

private: // channels & workers
  TrOneShotServer<events_comm::TrNativeEventMessage> *eventChanServer = nullptr;
  unique_ptr<WorkerThread> eventChanWatcher;

private: // Global configs
  struct RequestAuthorizationHeaders
  {
    // The raw headers string specified in the format of "Header-Name: Header-Value\r\nHeader-Name2: Header-Value2\r\n"
    std::string raw_headers;
    std::vector<std::string> allowed_origins;
  };
  RequestAuthorizationHeaders request_authorization_headers;
};
