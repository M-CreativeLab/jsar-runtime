#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <rapidjson/document.h>

// Forward declarations
class TrConstellation;
class TrContentRuntime;
class CdpHandler;
struct CdpMessage;

// Proxy class that forwards CDP requests to content processes via IPC
class ContentDomainProxy
{
public:
  // Callback type for async CDP responses
  using ResponseCallback = std::function<void(const std::string &response)>;

  ContentDomainProxy(TrConstellation *constellation, CdpHandler *cdpHandler);
  ~ContentDomainProxy();

  // Forward a CDP request to the appropriate content process (synchronous version)
  std::string forwardRequest(const std::string &method, const CdpMessage &message, const std::string &clientId);

  // Forward a CDP request to the appropriate content process (asynchronous version)
  void forwardRequestAsync(const std::string &method, const CdpMessage &message, const std::string &clientId, ResponseCallback callback);

  // Check if this domain should be handled by content processes
  bool shouldForwardDomain(const std::string &domain) const;

  // Handle response from content process (called by TrContentRuntime)
  void handleResponseFromContent(uint32_t requestId, const std::string &response);

private:
  // Structure to track pending requests
  struct PendingRequest
  {
    uint32_t requestId;
    uint32_t cdpMessageId; // Original CDP message ID
    std::string response;
    bool completed = false;
    std::condition_variable cv;
    ResponseCallback callback;                       // For async responses
    std::chrono::steady_clock::time_point timestamp; // For timeout tracking
  };

  TrConstellation *constellation_;
  CdpHandler *cdpHandler_; // Parent CDP handler that owns this proxy
  std::atomic<uint32_t> nextRequestId_;
  std::mutex pendingRequestsMutex_;
  std::unordered_map<uint32_t, std::unique_ptr<PendingRequest>> pendingRequests_;

  // Set of domains that should be forwarded to content processes
  std::unordered_set<std::string> forwardedDomains_;

  // Helper methods
  TrContentRuntime *findContentRuntime(const std::string &clientId);
  uint32_t generateRequestId();
  void handleResponse(uint32_t requestId, const std::string &response);
  void setupForwardedDomains();

  // Internal async forwarding implementation
  void forwardRequestInternal(const std::string &method, const CdpMessage &message, const std::string &clientId, ResponseCallback callback);

  // Send CDP request to content process via IPC
  bool sendCdpRequestToContent(TrContentRuntime *contentRuntime, uint32_t requestId, uint32_t cdpMessageId, const std::string &cdpMessageJson);

  // Timeout handling for pending requests
  void cleanupTimedOutRequests();
};