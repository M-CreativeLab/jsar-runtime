#pragma once

#include <memory>
#include <vector>
#include <shared_mutex>
#include <string>
#include <functional>

#include "common/options.hpp"
#include "common/scoped_thread.hpp"
#include "common/hive/message.hpp"
#include "common/hive/sender.hpp"
#include "./entry.hpp"

/**
 * The `TrHiveChildProcess` class represents a child process in the Hive server.
 */
class TrHiveChildProcess final
{
public:
  /**
   * Constructs a `TrHiveChildProcess` with a document ID.
   *
   * @param documentId The ID of the document associated with the child process.
   */
  explicit TrHiveChildProcess(uint32_t documentId);

  /**
   * Destructor for `TrHiveChildProcess`.
   */
  ~TrHiveChildProcess() = default;

public:
  /**
   * Forks the child process.
   */
  void fork();

  /**
   * Receives output from the child process.
   *
   * @param lineCallback A callback function to handle each line of output.
   */
  void recvOutput(const std::function<void(const std::string &)> &lineCallback);

  /**
   * Tests the child process.
   *
   * @return True if the child process is running, false otherwise.
   */
  bool test();

public:
  uint32_t documentId; // Document ID associated with the child process
  pid_t pid;           // Process ID of the child process
  int childPipes[2];   // Pipes for communication with the child process
  std::string lastOutput; // Last output from the child process
};

/**
 * The `TrHiveServer` class represents the Hive server.
 */
class TrHiveServer final
{
public:
  /**
   * Constructs a `TrHiveServer` with a client entry and port.
   *
   * @param clientEntry The client entry associated with the server.
   * @param port The port on which the server listens.
   */
  TrHiveServer(TrClientEntry *clientEntry, int port);

public:
  /**
   * Starts the Hive server.
   */
  void start();

  /**
   * Handles the creation of a new client.
   *
   * @param req The create client request.
   * @param sender The command sender.
   */
  void onCreateClient(hive_comm::TrCreateClientRequest &req, hive_comm::TrHiveCommandSender &sender);

  /**
   * Handles the termination of a client.
   *
   * @param req The terminate client request.
   * @param sender The command sender.
   */
  void onTerminateClient(hive_comm::TrTerminateClientRequest &req, hive_comm::TrHiveCommandSender &sender);

private:
  /**
   * Checks the status of the child processes.
   *
   * @param eventSender The command sender for sending status events.
   */
  void checkStatus(hive_comm::TrHiveCommandSender &eventSender);

public:
  TrClientEntry *clientEntry = nullptr; // Client entry associated with the server
  int port; // Port on which the server listens
  bool isChild = false; // Flag to indicate if the server is a child process

public: // Fields used by client
  TrDocumentRequestInit requestInit; // Request initialization data

private:
  bool running = false; // Flag to indicate if the server is running
  std::vector<std::shared_ptr<TrHiveChildProcess>> childProcesses; // List of child processes
  std::unique_ptr<WorkerThread> checkingStatusWorker; // Worker thread for checking status
  std::shared_mutex childProcessesMutex; // Mutex for thread-safe access to child processes
};
