#include "./content_domain_proxy.hpp"
#include "./cdp_handler.hpp" // For CdpMessage and CdpResponse
#include "../constellation.hpp"
#include "../content.hpp"
#include "../content_manager.hpp"
#include "common/debug.hpp"
#include "common/inspector/message.hpp"
#include "common/inspector/sender.hpp"
#include <chrono>
#include <unordered_set>

using namespace std;
using namespace inspector_comm;

// Static member definitions
mutex ContentDomainProxy::registryMutex_;
unordered_map<uint32_t, ContentDomainProxy *> ContentDomainProxy::requestIdToProxy_;

ContentDomainProxy::ContentDomainProxy(TrConstellation *constellation)
    : constellation_(constellation)
    , nextRequestId_(1)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Initialized");
  setupForwardedDomains();
}

ContentDomainProxy::~ContentDomainProxy()
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Destroyed");
}

string ContentDomainProxy::forwardRequest(const string &method, const CdpMessage &message, const string &clientId)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Synchronous forward request %s for client %s", method.c_str(), clientId.c_str());

  // Use a condition variable to wait for async response
  string result;
  bool responseReceived = false;
  mutex responseMutex;
  condition_variable responseCv;

  // Call the async version with a callback that stores the result
  forwardRequestAsync(method, message, clientId, [&](const string &response)
                      {
    lock_guard<mutex> lock(responseMutex);
    result = response;
    responseReceived = true;
    responseCv.notify_one(); });

  // Wait for response with timeout
  unique_lock<mutex> lock(responseMutex);
  auto timeout = chrono::steady_clock::now() + chrono::seconds(30); // 30 second timeout

  if (responseCv.wait_until(lock, timeout, [&]
                            { return responseReceived; }))
  {
    return result;
  }
  else
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Request timeout for method %s, client %s", method.c_str(), clientId.c_str());
    return CdpResponse::error(message.id, -32603, "Request timeout");
  }
}

void ContentDomainProxy::forwardRequestAsync(const string &method, const CdpMessage &message, const string &clientId, ResponseCallback callback)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Async forward request %s for client %s", method.c_str(), clientId.c_str());
  forwardRequestInternal(method, message, clientId, callback);
}

void ContentDomainProxy::forwardRequestInternal(const string &method, const CdpMessage &message, const string &clientId, ResponseCallback callback)
{
  // Find the content runtime for this client
  TrContentRuntime *contentRuntime = findContentRuntime(clientId);
  if (!contentRuntime)
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Content runtime not found for client %s", clientId.c_str());
    string errorResponse = CdpResponse::error(message.id, -32603, "Content process not available");
    if (callback)
      callback(errorResponse);
    return;
  }

  // Generate a unique request ID for tracking
  uint32_t requestId = generateRequestId();

  // Serialize the original CDP message
  rapidjson::Document doc;
  doc.SetObject();
  auto &allocator = doc.GetAllocator();

  doc.AddMember("id", rapidjson::Value().SetInt64(message.id), allocator);
  doc.AddMember("method", rapidjson::Value(method.c_str(), allocator), allocator);

  rapidjson::Value params(rapidjson::kObjectType);
  params.CopyFrom(message.params, allocator);
  doc.AddMember("params", params, allocator);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  string cdpMessageJson = buffer.GetString();

  // Create pending request tracker
  auto pendingRequest = make_unique<PendingRequest>();
  pendingRequest->requestId = requestId;
  pendingRequest->cdpMessageId = message.id;
  pendingRequest->callback = callback;
  pendingRequest->timestamp = chrono::steady_clock::now();

  {
    lock_guard<mutex> lock(pendingRequestsMutex_);
    pendingRequests_[requestId] = move(pendingRequest);
  }

  // Register request in static registry for callback
  registerRequest(requestId);

  // Send request to content process via inspector IPC channel
  bool success = sendCdpRequestToContent(contentRuntime, requestId, message.id, cdpMessageJson);
  if (!success)
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Failed to send request %u to content process", requestId);

    // Clean up pending request and return error
    {
      lock_guard<mutex> lock(pendingRequestsMutex_);
      pendingRequests_.erase(requestId);
    }

    // Remove from static registry
    unregisterRequest(requestId);

    string errorResponse = CdpResponse::error(message.id, -32603, "Failed to send request to content process");
    if (callback)
      callback(errorResponse);
  }
  else
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Request %u sent to content process, method: %s", requestId, method.c_str());
  }
}

bool ContentDomainProxy::sendCdpRequestToContent(TrContentRuntime *contentRuntime, uint32_t requestId, uint32_t cdpMessageId, const string &cdpMessageJson)
{
  if (!contentRuntime)
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Content runtime is null");
    return false;
  }

  // Create CDP request command to send to content process
  inspector_comm::TrCdpRequest cdpRequest(requestId, contentRuntime->id, cdpMessageJson);

  DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Sending CDP request %u to content runtime %d", requestId, contentRuntime->id);

  // Send the request via inspector IPC channel
  bool success = contentRuntime->sendInspectorCommand(cdpRequest);
  if (!success)
    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Failed to send CDP request %u to content runtime %d", requestId, contentRuntime->id);
  return success;
}
bool ContentDomainProxy::shouldForwardDomain(const string &domain) const
{
  return forwardedDomains_.find(domain) != forwardedDomains_.end();
}

TrContentRuntime *ContentDomainProxy::findContentRuntime(const string &clientId)
{
  DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Looking up content runtime for client %s", clientId.c_str());

  if (!constellation_ || !constellation_->contentManager)
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Constellation or content manager not available");
    return nullptr;
  }

  // Convert clientId to content ID (they should correspond to document/content IDs)
  uint32_t contentId;
  try
  {
    contentId = static_cast<uint32_t>(std::stoul(clientId));
  }
  catch (const std::exception &e)
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Invalid client ID format: %s", clientId.c_str());
    return nullptr;
  }

  // Look up the content runtime using the content manager
  auto contentRuntime = constellation_->contentManager->getContent(contentId);
  if (!contentRuntime)
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Content runtime not found for ID %u", contentId);
    return nullptr;
  }

  // Verify the content process is running and available
  if (!contentRuntime->available.load() || contentRuntime->shouldDestroy.load())
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Content runtime %u is not available or being destroyed", contentId);
    return nullptr;
  }

  DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Found content runtime %u for client %s", contentId, clientId.c_str());
  return contentRuntime.get();
}

uint32_t ContentDomainProxy::generateRequestId()
{
  return nextRequestId_.fetch_add(1);
}

void ContentDomainProxy::handleResponse(uint32_t requestId, const string &response)
{
  lock_guard<mutex> lock(pendingRequestsMutex_);
  auto it = pendingRequests_.find(requestId);
  if (it != pendingRequests_.end())
  {
    auto &pendingRequest = it->second;
    pendingRequest->response = response;
    pendingRequest->completed = true;

    // Call callback if provided (for async requests)
    if (pendingRequest->callback)
    {
      pendingRequest->callback(response);
    }

    // Notify any waiting synchronous requests
    pendingRequest->cv.notify_one();

    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Response received for request %u", requestId);
  }
  else
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Received response for unknown request %u", requestId);
  }
}

void ContentDomainProxy::cleanupTimedOutRequests()
{
  auto now = chrono::steady_clock::now();
  auto timeout = chrono::seconds(30); // 30 second timeout

  lock_guard<mutex> lock(pendingRequestsMutex_);
  auto it = pendingRequests_.begin();
  while (it != pendingRequests_.end())
  {
    auto &pendingRequest = it->second;
    if (!pendingRequest->completed && (now - pendingRequest->timestamp) > timeout)
    {
      DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Cleaning up timed out request %u", pendingRequest->requestId);

      // Call callback with timeout error if provided
      if (pendingRequest->callback)
      {
        string errorResponse = CdpResponse::error(pendingRequest->cdpMessageId, -32603, "Request timeout");
        pendingRequest->callback(errorResponse);
      }

      // Notify any waiting synchronous requests
      pendingRequest->cv.notify_one();

      // Remove from static registry
      unregisterRequest(pendingRequest->requestId);

      it = pendingRequests_.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

void ContentDomainProxy::setupForwardedDomains()
{
  // Define which domains should be forwarded to content processes
  // Initially, we'll forward Runtime and Example domains as examples
  forwardedDomains_.insert("Runtime");
  forwardedDomains_.insert("Example");
  forwardedDomains_.insert("Log");

  DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Configured %zu domains for forwarding", forwardedDomains_.size());
}

// Static method for content runtimes to call back with responses
void ContentDomainProxy::handleResponseFromContent(uint32_t requestId, const string &response)
{
  lock_guard<mutex> lock(registryMutex_);
  auto it = requestIdToProxy_.find(requestId);
  if (it != requestIdToProxy_.end())
  {
    ContentDomainProxy *proxy = it->second;
    // Call the instance method on the correct proxy
    proxy->handleResponse(requestId, response);
    // Remove from registry since response is handled
    requestIdToProxy_.erase(it);
  }
  else
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Static callback received response for unknown request %u", requestId);
  }
}

void ContentDomainProxy::registerRequest(uint32_t requestId)
{
  lock_guard<mutex> lock(registryMutex_);
  requestIdToProxy_[requestId] = this;
}

void ContentDomainProxy::unregisterRequest(uint32_t requestId)
{
  lock_guard<mutex> lock(registryMutex_);
  requestIdToProxy_.erase(requestId);
}