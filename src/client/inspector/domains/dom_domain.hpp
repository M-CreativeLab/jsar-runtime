#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <client/inspector/content_cdp_handler.hpp>
#include <client/dom/document.hpp>
#include <client/dom/node.hpp>
#include <client/dom/element.hpp>

namespace client_inspector::domains
{
  // CDP DOM node representation
  struct CdpNode
  {
    int nodeId;
    int parentId;
    int backendNodeId;
    int nodeType;
    std::string nodeName;
    std::string localName;
    std::string nodeValue;
    int childNodeCount;
    std::vector<CdpNode> children;
    std::vector<std::pair<std::string, std::string>> attributes;
    std::string documentURL;
    std::string baseURL;
    std::string publicId;
    std::string systemId;
    std::string xmlVersion;
  };

  // Content-side DOM domain implementation
  class CdpDomDomain : public ContentCdpDomainHandler
  {
  public:
    CdpDomDomain();
    ~CdpDomDomain() override = default;

    // ContentCdpDomainHandler interface
    std::string handleMethod(const std::string &method, const CdpMessage &message) override;
    std::string getDomainName() const override
    {
      return "DOM";
    }
    std::string getDomainDescription() const override
    {
      return "Content-side DOM domain for accessing and manipulating DOM";
    }

  private:
    // Method handlers
    std::string getDocument(const CdpMessage &message);
    std::string enable(const CdpMessage &message);
    std::string disable(const CdpMessage &message);

    // Helper methods
    CdpNode convertNodeToCdpNode(std::shared_ptr<dom::Node> node, int depth = 1);
    rapidjson::Value cdpNodeToJson(const CdpNode &node, rapidjson::Document::AllocatorType &allocator);
    int getNextNodeId();
    std::shared_ptr<dom::Document> getCurrentDocument();

    // State
    bool enabled_ = false;
    int nextNodeId_ = 1;
    std::unordered_map<int, std::shared_ptr<dom::Node>> nodeMap_;
  };
}