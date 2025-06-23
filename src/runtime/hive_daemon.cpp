#include <filesystem>
#include <assert.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "common/debug.hpp"
#include "common/options.hpp"
#include "./hive_daemon.hpp"
#include "./constellation.hpp"
#include "./content_manager.hpp"
#include "./embedder.hpp"

using namespace std;
using namespace std::filesystem;

TrHiveDaemon::TrHiveDaemon(TrConstellation *constellation)
    : constellation(constellation)
{
}

void TrHiveDaemon::start()
{
  runtimeDirectory = constellation->getOptions().runtimeDirectory();
  commandChanServer = make_unique<TrOneShotServer<hive_comm::TrHiveCommandMessage>>("HiveCommandChan");
  assert(commandChanServer != nullptr);
  assert(eventChanPort != 0);
  assert(mediaChanPort != 0);
  assert(commandBufferChanPort != 0);

  if (pipe(childPipes) == -1)
  {
    DEBUG(LOG_TAG_ERROR, "Failed to create child pipes for hive daemon.");
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

  daemonPid = ::fork();
  assert(daemonPid != -1);

  if (daemonPid == 0)
  {
    /**
     * Configure pipes for child process.
     */
    close(childPipes[0]); // close read pipe in child
    dup2(childPipes[1], STDOUT_FILENO);
    dup2(childPipes[1], STDERR_FILENO); // TODO: split stderr into another channel?
    close(childPipes[1]);

    onDeamonProcess();
  }
  else
  {
    /** Configure pipes for parent process  */
    close(childPipes[1]);
    DEBUG(LOG_TAG_CONTENT, "The hive process(%d) is started.", daemonPid);

    // The followings are only running at parent process
    recvWorker = make_unique<WorkerThread>("hiveCommandReceiver", [this](WorkerThread &_worker)
                                           { recvCommand(); });
    acceptChanClient(1500); // Wait 1.5s for the hive to connected
  }
}

void TrHiveDaemon::shutdown()
{
  if (daemonPid > 0)
  {
    kill(daemonPid, SIGKILL);
    daemonPid = -1;
  }
  if (recvWorker != nullptr)
  {
    recvWorker->stop();
    recvWorker.reset();
  }
  if (commandChanServer != nullptr)
    commandChanServer.reset();
}

bool TrHiveDaemon::createClient(TrDocumentRequestInit &requestInit, function<void(pid_t)> callback)
{
  if (commandSender == nullptr)
  {
    DEBUG(LOG_TAG_ERROR, "Skipped creating a client because the command sender is not initialized.");
    return false;
  }

  hive_comm::TrCreateClientRequest req(requestInit);
  {
    unique_lock<shared_mutex> lock(mutexForCreateProcessCallbacks);
    pendingCreateProcessCallbacks[req.documentId] = callback;
  }

  // Print the current thread id
  auto tid = std::this_thread::get_id();
  std::stringstream ss;
  ss << tid;
  std::string idStr = ss.str();
  DEBUG(LOG_TAG_CONTENT,
        "CreateClientRequest(%d) is sent from thread %s",
        req.documentId,
        idStr.c_str());

  if (commandSender->sendCommand(req))
  {
    DEBUG(LOG_TAG_CONTENT, "CreateClientRequest(%d) is dispatched successfully", req.documentId);
    return true;
  }
  else
  {
    DEBUG(LOG_TAG_ERROR, "Failed to dispatch CreateClientRequest(%d)", req.documentId);
    return false;
  }
}

bool TrHiveDaemon::terminateClient(uint32_t id)
{
  assert(commandSender != nullptr);

  auto tid = std::this_thread::get_id();
  std::stringstream ss;
  ss << tid;
  std::string idStr = ss.str();
  DEBUG(LOG_TAG_CONTENT,
        "TerminateClientRequest(%d) is sent from thread %s",
        id,
        idStr.c_str());

  hive_comm::TrTerminateClientRequest req(id);
  return commandSender->sendCommand(req);
}

void TrHiveDaemon::tick()
{
  recvOutput();
  if (!checkDaemonAlive())
  {
    DEBUG(LOG_TAG_ERROR, "The hive daemon is not alive, cleaning up and restarting...");
    cleanupDaemonResources();
    start();
    return;
  }

  // Check for new client
  acceptChanClient(0);
}

void TrHiveDaemon::onDeamonProcess()
{
  path basePath = path(runtimeDirectory);
  path clientPath = basePath / "TransmuteClient";
  auto embedder = constellation->getEmbedder();

  rapidjson::Document hiveConfig;
  hiveConfig.SetObject();

  auto &allocator = hiveConfig.GetAllocator();
  hiveConfig.AddMember("hiveChanPort", commandChanServer->getPort(), allocator);
  hiveConfig.AddMember("eventChanPort", eventChanPort, allocator);
  hiveConfig.AddMember("mediaChanPort", mediaChanPort, allocator);
  hiveConfig.AddMember("commandBufferChanPort", commandBufferChanPort, allocator);

  // Global settings
  auto &options = constellation->getOptions();
  auto applicationCacheDirectoryValue = rapidjson::Value(options.applicationCacheDirectory.c_str(), allocator);
  hiveConfig.AddMember("applicationCacheDirectory", applicationCacheDirectoryValue, allocator);
  auto httpsProxyServerValue = rapidjson::Value(options.httpsProxyServer.c_str(), allocator);
  hiveConfig.AddMember("httpsProxyServer", httpsProxyServerValue, allocator);
  hiveConfig.AddMember("enableV8Profiling", options.enableV8Profiling, allocator);

  // XR Device configuration
  auto xrDevice = constellation->xrDevice;
  if (xrDevice != nullptr)
  {
    rapidjson::Value xrDeviceObject(rapidjson::kObjectType);
    xrDeviceObject.AddMember("enabled", xrDevice->enabled(), allocator);
    xrDeviceObject.AddMember("active", true, allocator);
    xrDeviceObject.AddMember("stereoRenderingMode", static_cast<int>(xrDevice->getStereoRenderingMode()), allocator);
    xrDeviceObject.AddMember("commandChanPort", xrDevice->getCommandChanPort(), allocator);
    xrDeviceObject.AddMember("sessionContextZoneDirectory",
                             rapidjson::Value(xrDevice->getSessionContextZoneDirectory().c_str(), allocator),
                             allocator);
    xrDeviceObject.AddMember("deviceContextZonePath",
                             rapidjson::Value(xrDevice->getDeviceContextZonePath().c_str(), allocator),
                             allocator);
    xrDeviceObject.AddMember("inputSourcesZonePath",
                             rapidjson::Value(xrDevice->getInputSourcesZonePath().c_str(), allocator),
                             allocator);
    hiveConfig.AddMember("xrDevice", xrDeviceObject, allocator);
  }

  rapidjson::StringBuffer hiveConfigBuffer;
  rapidjson::Writer<rapidjson::StringBuffer> hiveConfigWriter(hiveConfigBuffer);
  hiveConfig.Accept(hiveConfigWriter);

  char *hiveArgs[] = {
    const_cast<char *>("jsar-client"),
    const_cast<char *>("--hive"),
    const_cast<char *>(hiveConfigBuffer.GetString()),
    nullptr};

  cout << "Starting hive process on \"" << clientPath.c_str() << "\"" << endl;
  if (execvp(clientPath.c_str(), hiveArgs) == -1)
    cerr << "Failed to start hive process on \"" << clientPath.c_str() << "\": " << strerror(errno) << endl;

  /**
   * NOTE: The following code will not be executed if `execvp()` is successful, thus the `exit()` does guarantee the child process
   * to be terminated specifically.
   */
  exit(0);
}

void TrHiveDaemon::onNewChanClient(TrOneShotClient<hive_comm::TrHiveCommandMessage> &client)
{
  if (commandReceiver != nullptr || commandSender != nullptr)
  {
    commandReceiver.reset();
    commandSender.reset();
  }
  commandReceiver = make_unique<hive_comm::TrHiveCommandReceiver>(&client);
  commandSender = make_unique<hive_comm::TrHiveCommandSender>(&client);
}

void TrHiveDaemon::acceptChanClient(int timeout)
{
  commandChanServer->tryAccept([this](TrOneShotClient<hive_comm::TrHiveCommandMessage> &client)
                               { onNewChanClient(client); },
                               timeout);
}

bool TrHiveDaemon::checkDaemonAlive()
{
  if (daemonPid <= 0)
    return false;

  int status;
  pid_t child = waitpid(daemonPid, &status, WNOHANG);
  if (child == -1)
  {
    DEBUG(LOG_TAG_ERROR, "Failed to wait for the hive(%d): %s", daemonPid, strerror(errno));
    return true;
  }
  else if (child > 0)
  {
    if (WIFEXITED(status) || WIFSTOPPED(status))
    {
      if (WIFEXITED(status)) // Exit
        DEBUG(LOG_TAG_ERROR, "The hive(%d) exits with code(%d)", daemonPid, WEXITSTATUS(status));
      else // Stopped
        DEBUG(LOG_TAG_ERROR, "The hive(%d) is stopped with a signal: %d", daemonPid, WSTOPSIG(status));
      return false;
    }
    else if (WIFSIGNALED(status))
    {
      // Process is terminated
      DEBUG(LOG_TAG_ERROR,
            "The hive(%d) is terminated by a signal: %d, and core dumped: %d",
            daemonPid,
            WTERMSIG(status),
            WCOREDUMP(status));
      return false;
    }
  }
  return true;
}

void TrHiveDaemon::cleanupDaemonResources()
{
  // Reset daemon state
  daemonPid = -1;
  daemonReady = false;

  // Close child pipes if they're open
  if (childPipes[0] != -1)
  {
    close(childPipes[0]);
    childPipes[0] = -1;
  }
  if (childPipes[1] != -1)
  {
    close(childPipes[1]);
    childPipes[1] = -1;
  }

  // Stop and reset command receiver worker
  if (recvWorker != nullptr)
  {
    recvWorker->stop();
    recvWorker.reset();
  }

  // Reset command channel components
  if (commandReceiver != nullptr)
    commandReceiver.reset();
  if (commandSender != nullptr)
    commandSender.reset();
  if (commandChanServer != nullptr)
    commandChanServer.reset();

  // Clear pending callbacks and notify them of failure
  pendingCreateProcessCallbacks.clear();

  DEBUG(LOG_TAG_CONTENT, "Hive daemon resources cleaned up.");
}

bool TrHiveDaemon::recvOutput()
{
  if (!started())
    return false;

  struct pollfd fds[1];
  fds[0].fd = childPipes[0];
  fds[0].events = POLLIN;

  int events = poll(fds, 1, 0);
  if (events <= 0)
    return false;

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
          DEBUG(LOG_TAG_ERROR, "Failed to read pipe from client(%d): %s", daemonPid, strerror(errno));
        break;
      }
      for (int pos = 0; pos < bytesRead; pos++)
      {
        if (buf[pos] == '\n')
        {
          DEBUG(LOG_TAG_CLIENT_ENTRY, "hive(%d): %s", daemonPid, lastOutput.c_str());
          lastOutput.clear();
        }
        else
        {
          lastOutput += buf[pos];
        }
      }
    } while (bytesRead > 0);
    return r;
  }
  else
  {
    return false;
  }
}

void TrHiveDaemon::recvCommand()
{
  if (commandReceiver == nullptr)
    return;

  hive_comm::TrHiveCommandMessage commandMessage;
  if (commandReceiver->recvCommand(commandMessage, 100))
  {
    switch (commandMessage.getType())
    {
    case hive_comm::TrHiveCommandType::CreateClientResponse:
    {
      auto res = hive_comm::TrHiveCommandBase::FromMessage<hive_comm::TrCreateClientResponse>(commandMessage);
      DEBUG(LOG_TAG_CONTENT, "<< CreateClientResponse(%d) with pid(%d)", res.documentId, res.pid);

      function<void(pid_t)> callback;
      {
        shared_lock<shared_mutex> lock(mutexForCreateProcessCallbacks);
        auto it = pendingCreateProcessCallbacks.find(res.documentId);
        if (it != pendingCreateProcessCallbacks.end())
        {
          callback = it->second;
          pendingCreateProcessCallbacks.erase(it);
        }
      }

      if (TR_LIKELY(callback))
        callback(res.pid);
      else
        DEBUG(LOG_TAG_ERROR, "No callback found for CreateClientResponse(%d)", res.documentId);
      break;
    }
    case hive_comm::TrHiveCommandType::TerminateClientResponse:
    {
      auto res = hive_comm::TrHiveCommandBase::FromMessage<hive_comm::TrTerminateClientResponse>(commandMessage);
      DEBUG(LOG_TAG_CONTENT, "<< TerminateClientResponse(%d)", res.documentId);
      // TODO: handle the response?
      break;
    }
    case hive_comm::TrHiveCommandType::OnServerReadyEvent:
    {
      daemonReady = true;
      break;
    }
    case hive_comm::TrHiveCommandType::OnExitEvent:
    {
      auto exitEvent = hive_comm::TrHiveCommandBase::FromMessage<hive_comm::TrOnExitEvent>(commandMessage);
      DEBUG(LOG_TAG_CONTENT, "<< OnExitEvent(%d) with code(%d)", exitEvent.documentId, exitEvent.code);

      auto contentToExit = constellation->contentManager->getContent(exitEvent.documentId, true);
      if (contentToExit != nullptr)
        contentToExit->onClientProcessExited(exitEvent.code);
      break;
    }
    case hive_comm::TrHiveCommandType::OnLogEntryEvent:
    {
      auto logEntry = hive_comm::TrHiveCommandBase::FromMessage<hive_comm::TrOnLogEntryEvent>(commandMessage);
      DEBUG(LOG_TAG_CLIENT_ENTRY, "client(%d): %s", logEntry.sourcePid, logEntry.text.c_str());
      break;
    }
    default:
      break;
    }
  }
}
