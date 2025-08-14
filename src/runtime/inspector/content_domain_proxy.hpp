#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <rapidjson/document.h>

// Forward declarations
class TrConstellation;
class TrContentRuntime;
struct CdpMessage;

// Proxy class that forwards CDP requests to content processes via IPC
class ContentDomainProxy
{
public:
  ContentDomainProxy(TrConstellation *constellation);
  ~ContentDomainProxy();

  // Forward a CDP request to the appropriate content process
  std::string forwardRequest(const std::string &method, const CdpMessage &message, const std::string &clientId);

  // Check if this domain should be handled by content processes
  bool shouldForwardDomain(const std::string &domain) const;

private:
  // Structure to track pending requests
  struct PendingRequest
  {
    uint32_t requestId;
    std::string response;
    bool completed = false;
    std::condition_variable cv;
  };

  TrConstellation *constellation_;
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
};