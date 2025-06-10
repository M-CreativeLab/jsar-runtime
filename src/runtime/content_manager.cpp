#include <filesystem>
#include <crates/jsar_jsbundle.h>

#include "./content_manager.hpp"
#include "./media_manager.hpp"
#include "./res/client.bin.h"

using namespace std;
using namespace std::filesystem;

TrContentManager::TrContentManager(TrConstellation *constellation)
    : constellation(constellation),
      hived(make_unique<TrHiveDaemon>(constellation))
{
  eventChanServer = new TrOneShotServer<events_comm::TrNativeEventMessage>("eventChan");

  auto eventTarget = constellation->nativeEventTarget;
  rpcRequestListener = eventTarget->addEventListener(events_comm::TrNativeEventType::RpcRequest, [this](auto type, auto event)
                                                     { onRpcRequest(event); });
  documentEventListener = eventTarget->addEventListener(events_comm::TrNativeEventType::DocumentEvent, [this](auto type, auto event)
                                                        { onDocumentEvent(event); });
}

TrContentManager::~TrContentManager()
{
  if (eventChanServer != nullptr)
  {
    delete eventChanServer;
    eventChanServer = nullptr;
  }
  DEBUG(LOG_TAG_CONTENT, "ContentManager(%p) is destroyed", this);
}

bool TrContentManager::initialize()
{
  installExecutable();
  installScripts();

  eventChanWatcher = make_unique<WorkerThread>("TrEventChanWatcher", [this](WorkerThread &)
                                               { acceptEventChanClients(); });
  return true;
}

bool TrContentManager::shutdown()
{
  enablePreContent = false;
  auto contentsCount = contents.size();
  {
    shared_lock<shared_mutex> lock(contentsMutex);
    for (auto content : contents)
      content->dispose();
  }

  while (true)
  {
    this_thread::sleep_for(chrono::milliseconds(100));
    tickOnFrame();
    if (contents.empty())
      break;
  }
  DEBUG(LOG_TAG_CONTENT, "All contents(%zu) has been disposed and removed", contentsCount);

  auto eventTarget = constellation->nativeEventTarget;
  if (eventTarget != nullptr)
  {
    eventTarget->removeEventListener(events_comm::TrNativeEventType::RpcRequest, rpcRequestListener);
    eventTarget->removeEventListener(events_comm::TrNativeEventType::DocumentEvent, documentEventListener);
  }

  eventChanWatcher->stop();
  hived->shutdown();
  DEBUG(LOG_TAG_CONTENT, "TrContentManager::shutdown() done.");
  return true;
}

bool TrContentManager::tickOnFrame()
{
  hived->tick();

  // When the hive daemon is ready, we need to make sure the pre-content is always ready.
  if (enablePreContent && hived->daemonReady)
    preparePreContent();

  {
    // Check the status of each content runtime.
    shared_lock<shared_mutex> lock(contentsMutex);
    for (auto content : contents)
    {
      if (content->available)
        content->tickOnFrame();
    }
  }

  /**
   * Check if there is contents that need to be destroyed.
   */
  onTryDestroyingContents();
  return true;
}

shared_ptr<TrContentRuntime> TrContentManager::makeContent()
{
  shared_ptr<TrContentRuntime> contentToUse;
  {
    unique_lock<shared_mutex> lock(contentsMutex);
    for (auto content : contents)
    {
      if (!content->used)
      {
        content->used = true;
        contentToUse = content;
        break;
      }
    }
  }
  if (contentToUse == nullptr)
  {
    // Create a new content runtime when there is no available content.
    contentToUse = TrContentRuntime::Make(this);
    {
      unique_lock<shared_mutex> lock(contentsMutex);
      contentToUse->used = true;
      contents.push_back(contentToUse);
    }
  }
  return contentToUse;
}

shared_ptr<TrContentRuntime> TrContentManager::getContent(uint32_t id, bool includePreContent)
{
  shared_lock<shared_mutex> lock(contentsMutex);
  for (auto it = contents.begin(); it != contents.end(); ++it)
  {
    auto content = *it;
    if (content->id == id)
    {
      if (includePreContent || content->started)
        return content;
      else
        break;
    }
  }
  return nullptr;
}

shared_ptr<TrContentRuntime> TrContentManager::findContentByPid(pid_t pid)
{
  shared_lock<shared_mutex> lock(contentsMutex);
  for (auto it = contents.begin(); it != contents.end(); ++it)
  {
    auto content = *it;
    if (content->pid == pid)
      return content;
  }
  return nullptr;
}

void TrContentManager::disposeContent(shared_ptr<TrContentRuntime> content)
{
  unique_lock<shared_mutex> lock(contentsMutex);
  auto it = std::find(contents.begin(), contents.end(), content);
  if (it != contents.end())
    contents.erase(it);
}

void TrContentManager::disposeAll()
{
  unique_lock<shared_mutex> lock(contentsMutex);
  for (auto it = contents.begin(); it != contents.end(); ++it)
  {
    auto content = *it;
    // Only dispose the content that is used.
    if (content->used)
      content->dispose();
  }
}

void TrContentManager::onNewClientOnEventChan(TrOneShotClient<events_comm::TrNativeEventMessage> &client)
{
  auto peerId = client.getCustomId();
  auto content = getContent(peerId, true);
  if (content == nullptr)
  {
    eventChanServer->removeClient(&client);
    DEBUG(LOG_TAG_CONTENT, "Failed to accept a new event chan client: could not find #%d from contents", peerId);
  }
  else
  {
    content->onEventChanConnected(client);
  }
}

void TrContentManager::onTryDestroyingContents()
{
  bool hasContentsToRemove = false;
  {
    /**
     * Use a shared lock to check if there is a content that should be destroyed, this avoids the lock contention
     * at frame tick.
     *
     * TODO: Use a lock-free queue to simplify?
     */
    shared_lock<shared_mutex> lock(contentsMutex);
    for (auto content : contents)
    {
      if (content->shouldDestroy)
      {
        hasContentsToRemove = true;
        break;
      }
    }
  }

  if (hasContentsToRemove)
  {
    /**
     * Only if there is a content that should be removed, then we need a unique lock to destroy it.
     */
    unique_lock<shared_mutex> lock(contentsMutex);
    for (auto it = contents.begin(); it != contents.end();)
    {
      shared_ptr<TrContentRuntime> content = *it;
      if (content->shouldDestroy)
        it = contents.erase(it);
      else
        ++it;
    }
  }
}

void TrContentManager::onRpcRequest(std::shared_ptr<events_comm::TrNativeEvent> event)
{
  if (TR_UNLIKELY(event->type != events_comm::TrNativeEventType::RpcRequest))
    return;

  auto detail = event->detail<events_comm::TrRpcRequest>();
  auto content = getContent(detail.documentId, true);
  if (TR_UNLIKELY(content == nullptr))
  {
    DEBUG(LOG_TAG_ERROR, "Failed to find the content(%d) for the RpcRequest", detail.documentId);
    return;
  }
  constellation->dispatchNativeEvent(*event, content);
}

void TrContentManager::onDocumentEvent(std::shared_ptr<events_comm::TrNativeEvent> event)
{
  if (TR_UNLIKELY(event->type != events_comm::TrNativeEventType::DocumentEvent))
    return;

  auto detail = event->detail<events_comm::TrDocumentEvent>();
  auto content = getContent(detail.documentId, true);
  if (TR_UNLIKELY(content == nullptr))
  {
    DEBUG(LOG_TAG_ERROR, "Failed to find the content(%d) for the DocumentEvent", detail.documentId);
    return;
  }
  content->logDocumentEvent(detail);
  constellation->dispatchNativeEvent(*event, content);
}

/**
 * Install the executable to the runtime directory.
 *
 * @param runtimeDir The runtime directory to install the executable.
 * @param executableName The name of the executable.
 * @param writeContent The function to write the content to the file.
 */
static void InstallExecutable(string runtimeDir, string executableName, string executableMd5,
                              std::function<void(FILE *)> writeContent)
{
  path execPath = path(runtimeDir) / executableName;
  path execMd5Path = execPath.string() + ".md5";
  bool shouldInstall = false;

  /**
   * Check if the executable file exists, if not, we should always install it.
   */
  if (filesystem::exists(execPath))
  {
    /**
     * If the MD5 file doesn't exist, we should install the executable.
     */
    if (!filesystem::exists(execMd5Path))
    {
      shouldInstall = true;
    }
    else
    {
      /**
       * Otherwise, we need to compare the MD5 hash of the executable content.
       */
      FILE *md5fp = fopen(execMd5Path.c_str(), "rb");
      if (md5fp != nullptr)
      {
        char md5[33];
        fread(md5, 1, 32, md5fp);
        md5[32] = '\0';
        fclose(md5fp);

        /**
         * If the MD5 hash is different, we should install the executable.
         */
        if (string(md5) != executableMd5)
        {
          shouldInstall = true;
        }
      }
      else
      {
        /**
         * If the MD5 file is not readable, we should install the executable.
         */
        shouldInstall = true;
      }
    }

    /**
     * When the executable is already installed and the md5 hash is also verified, we need to check if the file's permission.
     */
    if (!shouldInstall)
    {
      struct stat st;
      if (stat(execPath.c_str(), &st) != 0)
      {
        auto msg = "Failed to stat() on the executable file";
        DEBUG(LOG_TAG_ERROR, "%s: %s", msg, strerror(errno));
        throw runtime_error(msg);
      }

      /**
       * Skip the installation when both the file are verified and executable.
       */
      if ((st.st_mode & S_IXUSR) != 0)
        return;
    }
  }
  else
  {
    shouldInstall = true;
  }

  /**
   * Install the executable file and the MD5 hash.
   */
  if (shouldInstall)
  {
    DEBUG(LOG_TAG_CONTENT, "Installing the executable file: %s", execPath.c_str());

    // Write the file content.
    FILE *fp = fopen(execPath.c_str(), "wb");
    if (fp != nullptr)
    {
      writeContent(fp);
      fclose(fp);
    }

    // Write the MD5 hash of the content.
    FILE *md5fp = fopen(execMd5Path.c_str(), "wb");
    if (md5fp != nullptr)
    {
      fwrite(executableMd5.c_str(), 1, executableMd5.size(), md5fp);
      fclose(md5fp);
    }
  }

  // Make the library executable.
  if (chmod(execPath.c_str(), 0755) == -1)
  {
    auto msg = "Failed to chmod for the executable file";
    DEBUG(LOG_TAG_ERROR, "%s: %s", msg, strerror(errno));
    throw runtime_error(msg);
  }
}

/**
 * Install the Node.js library to the target directory.
 *
 * @param runtimeDir The runtime directory to install the library.
 */
static void InstallNodejsLibrary(string runtimeDir)
{
  string name;
#ifdef __APPLE__
  name = "libnode.108.dylib";
#elif defined(__ANDROID__)
  name = "libnode.so";
#else
  throw runtime_error("Unsupported platform to install the Node.js library");
#endif

  string md5 = string(reinterpret_cast<const char *>(get_libnode_md5_ptr()), get_libnode_md5_size());
  return InstallExecutable(runtimeDir, name, md5, [](FILE *fp)
                           { fwrite(get_libnode_ptr(), 1, get_libnode_size(), fp); });
}

void TrContentManager::installExecutable()
{
  auto executableTargetDir = constellation->getOptions().runtimeDirectory();
  if (!filesystem::exists(executableTargetDir))
    filesystem::create_directory(executableTargetDir);

  // Install the Node.js library.
  InstallNodejsLibrary(executableTargetDir);

  // Install the TransmuteClient executable.
  {
    auto writeFile = [](FILE *fp)
    {
      shared_ptr<carbonite::StringReference> contents = carbonite::decompressBinary(transmute_client_binary,
                                                                                    transmute_client_binary_len);
      if (contents != nullptr)
      {
        fwrite(contents->data(), 1, contents->size(), fp);
        contents.reset();
      }
      else
      {
        throw runtime_error("Failed to decompress the TransmuteClient binary");
      }
    };
    InstallExecutable(executableTargetDir,
                      "TransmuteClient",
                      string(transmute_client_binary_md5),
                      writeFile);
  }
}

void TrContentManager::installScripts()
{
  auto scriptsTargetDir = constellation->getOptions().scriptsDirectory();
  if (!filesystem::exists(scriptsTargetDir))
    filesystem::create_directory(scriptsTargetDir);

#define INSTALL_BOOTSTRAP_SCRIPT(framework, scriptName)                                         \
  {                                                                                             \
    auto sourcePtr = JSBundle::GetBootstrapSourcePtr(JSFrameworkName::framework);               \
    path sourcePath = path(scriptsTargetDir) / scriptName;                                      \
    {                                                                                           \
      FILE *fp = fopen(sourcePath.c_str(), "wb");                                               \
      if (fp != nullptr)                                                                        \
      {                                                                                         \
        fwrite(sourcePtr, 1, JSBundle::GetBootstrapSourceSize(JSFrameworkName::framework), fp); \
        fclose(fp);                                                                             \
      }                                                                                         \
    }                                                                                           \
  }

  INSTALL_BOOTSTRAP_SCRIPT(BABYLON, "jsar-bootstrap-babylon.js");
#undef INSTALL_BOOTSTRAP_SCRIPT

#define INSTALL_JSBUNDLE_SCRIPT(id, scriptName)                                      \
  {                                                                                  \
    auto sourcePtr = JSBundle::GetClientEntrySourcePtr(JSBundles::id);               \
    path sourcePath = path(scriptsTargetDir) / scriptName;                           \
    {                                                                                \
      FILE *fp = fopen(sourcePath.c_str(), "wb");                                    \
      if (fp != nullptr)                                                             \
      {                                                                              \
        fwrite(sourcePtr, 1, JSBundle::GetClientEntrySourceSize(JSBundles::id), fp); \
        fclose(fp);                                                                  \
      }                                                                              \
    }                                                                                \
  }

  INSTALL_JSBUNDLE_SCRIPT(MainEntry, "jsar-client-entry.js");
  INSTALL_JSBUNDLE_SCRIPT(WebWorkersEntry, "jsar-webworkers-entry.js");
#undef INSTALL_JSBUNDLE_SCRIPT
}

void TrContentManager::startHived()
{
  {
    // Configure the hived
    hived->eventChanPort = eventChanServer->getPort();
    hived->mediaChanPort = constellation->mediaManager->chanPort();
    hived->commandBufferChanPort = constellation->renderer->getCommandBufferChanPort();
  }
  hived->start();
}

void TrContentManager::preparePreContent()
{
  if (preContentScheduled)
  {
    if (chrono::system_clock::now() < preContentScheduledTimepoint)
      return;

    auto preContent = TrContentRuntime::Make(this);
    {
      unique_lock<shared_mutex> lock(contentsMutex);
      contents.push_back(preContent);
    }
    preContent->preStart();
    preContentScheduled = false;
    return;
  }

  bool hasPreContent = false;
  bool hasContents = false;
  {
    shared_lock<shared_mutex> lock(contentsMutex);
    hasContents = !contents.empty();
    if (hasContents)
    {
      for (auto content : contents)
      {
        if (!content->used)
        {
          hasPreContent = true;
          break;
        }
      }
    }
  }

  if (!hasPreContent)
  {
    // TODO: support the pre-content configuration?
    int delayTime = hasContents ? 3000 : 0;
    preContentScheduledTimepoint = chrono::system_clock::now() + chrono::milliseconds(delayTime);
    preContentScheduled = true;
  }
}

void TrContentManager::acceptEventChanClients(int timeout)
{
  eventChanServer->tryAccept([this](TrOneShotClient<events_comm::TrNativeEventMessage> &newClient)
                             { onNewClientOnEventChan(newClient); }, timeout);
}
