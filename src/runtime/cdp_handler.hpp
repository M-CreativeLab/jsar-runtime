#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <rapidjson/document.h>

class TrInspectorClient;

// CDP (Chrome DevTools Protocol) Message Structure
struct CdpMessage {
    int id = -1;
    std::string method;
    rapidjson::Value params;
    
    static std::unique_ptr<CdpMessage> parse(const std::string& json);
};

// CDP Response Builder
class CdpResponse {
public:
    static std::string success(int id, const rapidjson::Value& result);
    static std::string error(int id, int code, const std::string& message);
    static std::string event(const std::string& method, const rapidjson::Value& params);
};

// Base CDP Domain Handler
class CdpDomainHandler {
public:
    virtual ~CdpDomainHandler() = default;
    virtual std::string handleMethod(const std::string& method, const CdpMessage& message) = 0;
};

// CDP Handler - Main coordinator for CDP message processing
class CdpHandler {
public:
    CdpHandler();
    ~CdpHandler();

    // Process incoming CDP message and return response
    std::string processMessage(const std::string& message);
    
    // Register a domain handler
    void registerDomain(const std::string& domain, std::unique_ptr<CdpDomainHandler> handler);

private:
    std::unordered_map<std::string, std::unique_ptr<CdpDomainHandler>> domains_;
    
    std::string extractDomain(const std::string& method);
    std::string extractMethodName(const std::string& method);
};