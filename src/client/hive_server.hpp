#pragma once

#include <memory>
#include <vector>
#include <shared_mutex>

#include "common/options.hpp"
#include "common/scoped_thread.hpp"
#include "common/hive/message.hpp"
#include "common/hive/sender.hpp"
#include "./entry.hpp"

class TrHiveChildProcess final
{
public:
  TrHiveChildProcess(uint32_t documentId);
  ~TrHiveChildProcess() = default;

public:
  void fork();
  void recvOutput(std::function<void(const string &)> lineCallback);
  bool test();

public:
  uint32_t documentId;
  pid_t pid;
  int childPipes[2];
  string lastOutput;
};

class TrHiveServer final
{
public:
  TrHiveServer(TrClientEntry *clientEntry, int port);

public:
  void start();
  void onCreateClient(hive_comm::TrCreateClientRequest &req, hive_comm::TrHiveCommandSender &sender);
  void onTerminateClient(hive_comm::TrTerminateClientRequest &req, hive_comm::TrHiveCommandSender &sender);

private:
  void checkStatus(hive_comm::TrHiveCommandSender &eventSender);

public:
  TrClientEntry *clientEntry = nullptr;
  int port;
  bool isChild = false;

public: // fields used by client
  TrDocumentRequestInit requestInit;

private:
  bool running = false;
  vector<shared_ptr<TrHiveChildProcess>> childProcesses;
  unique_ptr<WorkerThread> checkingStatusWorker;
  shared_mutex childProcessesMutex;
};
