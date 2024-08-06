#include <sys/wait.h>
#include "common/hive/message.hpp"
#include "common/hive/sender.hpp"
#include "common/hive/receiver.hpp"
#include "./hive_server.hpp"
#include "./entry.hpp"

TrHiveChildProcess::TrHiveChildProcess(uint32_t documentId) : documentId(documentId)
{
  if (pipe(childPipes) == -1)
  {
    fprintf(stderr, "Failed to create child pipes for hive daemon.");
  }
  else
  {
    int flags = fcntl(childPipes[0], F_GETFL, 0);
    if (flags != -1)
    {
      flags |= O_NONBLOCK;
      fcntl(childPipes[0], F_SETFL, flags);
    }
  }

  pid = ::fork();
  if (pid == 0)
  {
    close(childPipes[0]);
    dup2(childPipes[1], STDOUT_FILENO);
    dup2(childPipes[1], STDERR_FILENO);
    close(childPipes[1]);
  }
  else if (pid > 0)
  {
    close(childPipes[1]);
  }
}

void TrHiveChildProcess::recvOutput(std::function<void(const string &)> lineCallback)
{
  struct pollfd fds[1];
  fds[0].fd = childPipes[0];
  fds[0].events = POLLIN;

  int events = poll(fds, 1, 0);
  if (events <= 0)
    return;

  char buf[2048];
  if (fds[0].revents & POLLIN)
  {
    bool r = true;
    ssize_t bytesRead = 0;
    do
    {
      bytesRead = read(childPipes[0], buf, sizeof(buf));
      if (bytesRead <= 0)
      {
        r = false;
        if (bytesRead == -1 && (errno != EAGAIN && errno != EWOULDBLOCK))
          DEBUG(LOG_TAG_ERROR, "Failed to read pipe from client(%d): %s", documentId, strerror(errno));
        break;
      }
      for (int pos = 0; pos < bytesRead; pos++)
      {
        if (buf[pos] == '\n')
        {
          lineCallback(lastOutput);
          lastOutput.clear();
        }
        else
        {
          lastOutput += buf[pos];
        }
      }
    } while (bytesRead > 0);
    return;
  }
  else
  {
    return;
  }
}

bool TrHiveChildProcess::test()
{
  int status;
  pid_t child = waitpid(pid, &status, WNOHANG);
  if (child == -1)
  {
    fprintf(stderr, "Failed to wait for the client process(%d): %s\n", pid, strerror(errno));
    return true;
  }
  else if (child > 0)
  {
    if (WIFEXITED(status) || WIFSTOPPED(status))
    {
      if (WIFEXITED(status)) // Exit
        fprintf(stderr, "The client process(%d) exits with code(%d)\n", pid, WEXITSTATUS(status));
      else // Stopped
        fprintf(stderr, "The client process(%d) is stopped with a signal: %d\n", pid, WSTOPSIG(status));
      pid = -1;
      return false;
    }
    else if (WIFSIGNALED(status))
    {
      // Process is terminated
      fprintf(stderr, "The client process(%d) is terminated by a signal: %d, and core dumped: %d\n",
              pid, WTERMSIG(status), WCOREDUMP(status));
      pid = -1;
      return false;
    }
  }
  return true;
}

TrHiveServer::TrHiveServer(TrClientEntry *clientEntry, int port) : clientEntry(clientEntry), port(port)
{
  assert(clientEntry != nullptr);
  assert(port > 0);
}

void TrHiveServer::start()
{
  auto commandChanClient = TrOneShotClient<hive_comm::TrHiveCommandMessage>::MakeAndConnect(port, false);
  assert(commandChanClient != nullptr);

  hive_comm::TrHiveCommandReceiver commandReceiver(commandChanClient);
  hive_comm::TrHiveCommandSender commandSender(commandChanClient);
  checkingStatusWorker = make_unique<WorkerThread>("HiveChildrenStatus", [&, this](WorkerThread &worker)
                                                   { checkStatus(commandSender); worker.sleep(); }, 100);

  running = true;
  fprintf(stdout, "Hive daemon is started, and listening process creation commands.\n");

  while (true)
  {
    hive_comm::TrHiveCommandMessage commandMessage;
    if (commandReceiver.recvCommand(commandMessage, 100))
    {
      switch (commandMessage.getType())
      {
      case hive_comm::TrHiveCommandType::CreateClientRequest:
      {
        auto createProcessReq = hive_comm::TrHiveCommandBase::FromMessage<hive_comm::TrCreateClientRequest>(commandMessage);
        onCreateClient(createProcessReq, commandSender);
        break;
      }
      case hive_comm::TrHiveCommandType::TerminateClientRequest:
      {
        auto terminateProcessReq = hive_comm::TrHiveCommandBase::FromMessage<hive_comm::TrTerminateClientRequest>(commandMessage);
        onTerminateClient(terminateProcessReq, commandSender);
        break;
      }
      default:
        break;
      }
    }

    // The running flag could be set by the above functions.
    if (!running)
      break;
  }
}

void TrHiveServer::onCreateClient(hive_comm::TrCreateClientRequest &req, hive_comm::TrHiveCommandSender &sender)
{
  hive_comm::TrCreateClientResponse createProcessRes(req);
  auto childProcess = make_shared<TrHiveChildProcess>(req.documentId);
  if (childProcess == nullptr || childProcess->pid < 0)
  {
    fprintf(stderr, "Failed to create a child process for document(%d)\n", req.documentId);
    sender.sendCommand(createProcessRes);
    return;
  }

  if (childProcess->pid != 0)
  {
    unique_lock<shared_mutex> lock(childProcessesMutex);
    childProcesses.push_back(childProcess);
    createProcessRes.pid = childProcess->pid;
    sender.sendCommand(createProcessRes);
  }
  else
  {
    {
      requestInit.id = req.documentId;
      requestInit.url = req.url;
      requestInit.disableCache = req.disableCache;
      requestInit.isPreview = req.isPreview;
      requestInit.runScripts = req.runScripts;
    }
    isChild = true;
    running = false;
  }
}

void TrHiveServer::onTerminateClient(hive_comm::TrTerminateClientRequest &req, hive_comm::TrHiveCommandSender &sender)
{
  unique_lock<shared_mutex> lock(childProcessesMutex);
  for (auto childProcess : childProcesses)
  {
    if (childProcess->documentId == req.documentId)
    {
      kill(childProcess->pid, SIGKILL);
      fprintf(stdout, "The process(%d) has been stopped forcily\n", childProcess->pid);

      // Respond to the host process
      hive_comm::TrTerminateClientResponse res(req, true);
      sender.sendCommand(res);
    }
  }
}

void TrHiveServer::checkStatus(hive_comm::TrHiveCommandSender &eventSender)
{
  unique_lock<shared_mutex> lock(childProcessesMutex);
  for (auto it = childProcesses.begin(); it != childProcesses.end();)
  {
    auto &childProcess = *it;
    childProcess->recvOutput([&](const string &line)
                             { hive_comm::TrOnLogEntryEvent logEntry(childProcess->documentId, childProcess->pid);
                               logEntry.text = line;
                               eventSender.sendCommand(logEntry); });

    auto chid = childProcess->documentId;
    auto childPid = childProcess->pid;
    if (!childProcess->test())
    {
      {
        // Send "exit" event
        hive_comm::TrOnExitEvent exitEvent(childProcess->documentId, 1);
        eventSender.sendCommand(exitEvent);
      }
      it = childProcesses.erase(it);
      fprintf(stdout, "The exited ChildProcess(#%d|%d) has been removed from managed list.\n", chid, childPid);
    }
    else
      ++it;
  }
}
