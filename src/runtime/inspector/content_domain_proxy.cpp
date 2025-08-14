#include "./content_domain_proxy.hpp"
#include "./cdp_handler.hpp" // For CdpMessage and CdpResponse
#include "../constellation.hpp"
#include "../content.hpp"
#include "common/debug.hpp"
#include "common/inspector/message.hpp"
#include "common/inspector/sender.hpp"
#include <chrono>
#include <unordered_set>

using namespace std;
using namespace inspector_comm;

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
  DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Forwarding request %s for client %s", method.c_str(), clientId.c_str());

  // Find the content runtime for this client
  TrContentRuntime *contentRuntime = findContentRuntime(clientId);
  if (!contentRuntime)
  {
    DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Content runtime not found for client %s", clientId.c_str());
    return CdpResponse::error(message.id, -32603, "Content process not available");
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

  {
    lock_guard<mutex> lock(pendingRequestsMutex_);
    pendingRequests_[requestId] = move(pendingRequest);
  }

  // TODO: Send request to content process via inspector IPC channel
  // This implementation will be completed when the full IPC integration is ready.
  // The infrastructure is in place with:
  // - Inspector IPC message types in src/common/inspector/message.hpp
  // - Content-side handlers in src/client/inspector/
  // - Proxy forwarding logic here
  //
  // Future implementation should:
  // 1. Look up the content runtime for the clientId
  // 2. Send TrCdpRequest via inspector IPC channel
  // 3. Wait for TrCdpResponse from content process
  // 4. Return the response to the CDP client
  //
  // For now, return a placeholder response indicating the proxy is working
  DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Request %u prepared for content process, method: %s", requestId, method.c_str());

  // Simulate processing and return a response indicating proxy functionality
  rapidjson::Document result;
  result.SetObject();
  result.AddMember("proxied", rapidjson::Value().SetBool(true), allocator);
  result.AddMember("method", rapidjson::Value(method.c_str(), allocator), allocator);
  result.AddMember("requestId", rapidjson::Value().SetUint(requestId), allocator);
  result.AddMember("contentId", rapidjson::Value(clientId.c_str(), allocator), allocator);
  result.AddMember("status", rapidjson::Value("forwarded_to_content_process", allocator), allocator);

  // Clean up pending request
  {
    lock_guard<mutex> lock(pendingRequestsMutex_);
    pendingRequests_.erase(requestId);
  }

  return CdpResponse::success(message.id, result);
}

bool ContentDomainProxy::shouldForwardDomain(const string &domain) const
{
  return forwardedDomains_.find(domain) != forwardedDomains_.end();
}

TrContentRuntime *ContentDomainProxy::findContentRuntime(const string &clientId)
{
  // TODO: Implement proper content runtime lookup
  // This should integrate with TrConstellation to find the content runtime
  // associated with the given clientId. The clientId typically corresponds
  // to the content/document ID that the CDP client is debugging.
  //
  // Implementation should:
  // 1. Use constellation_->getContentManager() to access content runtimes
  // 2. Find the content runtime matching the clientId
  // 3. Verify the content process is running and has inspector channel
  //
  // For now, this is a placeholder implementation
  DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Looking up content runtime for client %s", clientId.c_str());
  return nullptr;
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
    it->second->response = response;
    it->second->completed = true;
    it->second->cv.notify_one();
  }
}

void ContentDomainProxy::setupForwardedDomains()
{
  // Define which domains should be forwarded to content processes
  // Initially, we'll forward Runtime and Example domains as examples
  forwardedDomains_.insert("Runtime");
  forwardedDomains_.insert("Example");

  DEBUG(LOG_TAG_INSPECTOR, "CDP Proxy: Configured %zu domains for forwarding", forwardedDomains_.size());
}