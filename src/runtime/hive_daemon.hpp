#pragma once

#include <vector>
#include <map>
#include <string>
#include <shared_mutex>
#include <unistd.h>
#include "common/classes.hpp"
#include "common/scoped_thread.hpp"
#include "common/ipc.hpp"
#include "common/hive/message.hpp"
#include "common/hive/sender.hpp"
#include "common/hive/receiver.hpp"

/**
 * The Hive Daemon is a process that manages the creation of content processes.
 */
class TrHiveDaemon final
{
  friend class TrContentManager;

public:
  /**
   * Construct a new hive daemon.
   * 
   * @param constellation The constellation to create the hive daemon.
   */
  TrHiveDaemon(TrConstellation *constellation);
  ~TrHiveDaemon() = default;

public:
  /**
   * Check if the hive daemon process is started.
   */
  inline bool started() { return daemonPid > 0; }
  /**
   * Start the hive daemon process, and wait for it's ready for incubating contents.
   */
  void start();
  /**
   * Shutdown the hive daemon process.
   */
  void shutdown();
  /**
   * Creating a new client with the request.
   *
   * NOTE: The callback will be called at event thread, so the callback is thread-unsafe, be cautious to write the
   * callback.
   *
   * @param requestInit The request to create a client.
   * @param callback The callback to be called when the client is created.
   */
  bool createClient(TrDocumentRequestInit &requestInit, function<void(pid_t)> callback);
  /**
   * Terminate a client with its id.
   *
   * @param clientId The id of the client to be terminated.
   * @returns `true` if the client is terminated.
   */
  bool terminateClient(uint32_t clientId);
  /**
   * At tick(), it does:
   *
   * - Check if the daemon process is still alive, if not, restart it.
   * - Try to accept a new chan client if there is no client connected.
   * - Try to receive the output from the daemon process.
   */
  void tick();

private:
  /**
   * The main of the daemon process.
   */
  void onDeamonProcess();
  /**
   * Handle the new client connected to the command channel.
   * 
   * @param client The new client connected to the command channel.
   */
  void onNewChanClient(TrOneShotClient<hive_comm::TrHiveCommandMessage> &client);
  /**
   * Accept the chan client.
   * 
   * @param timeout The timeout to accept the chan client.
   */
  void acceptChanClient(int timeout);
  /**
   * Check if the daemon process is still alive.
   *
   * @return true if the daemon process is alive.
   */
  bool checkDaemonAlive();
  /**
   * Try to receive the output from the daemon process.
   *
   * @returns true if the output is received.
   */
  bool recvOutput();
  /**
   * Receive a command from the channel.
   */
  void recvCommand();

public:
  pid_t daemonPid;
  std::atomic<bool> daemonReady = false;
  std::vector<pid_t> childPids;
  int childPipes[2];
  std::string runtimeDirectory;
  std::string lastOutput;

public: // fields to share for hived
  int eventChanPort;
  int frameChanPort;
  int mediaChanPort;
  int commandBufferChanPort;

private:
  TrConstellation *constellation = nullptr;
  std::unique_ptr<ipc::TrOneShotServer<hive_comm::TrHiveCommandMessage>> commandChanServer = nullptr;
  std::unique_ptr<hive_comm::TrHiveCommandReceiver> commandReceiver = nullptr;
  std::unique_ptr<hive_comm::TrHiveCommandSender> commandSender = nullptr;
  std::unique_ptr<WorkerThread> recvWorker = nullptr;
  std::map<uint32_t, function<void(pid_t)>> pendingCreateProcessCallbacks;
  std::shared_mutex mutexForCreateProcessCallbacks;
};
