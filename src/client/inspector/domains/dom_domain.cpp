#include <rapidjson/document.h>
#include <client/per_process.hpp>
#include "./dom_domain.hpp"

namespace client_inspector::domains
{
  using namespace std;

  CdpDomDomain::CdpDomDomain()
  {
  }

  string CdpDomDomain::handleMethod(const string &method, const CdpMessage &message)
  {
    if (method == "getDocument")
    {
      return getDocument(message);
    }
    else if (method == "enable")
    {
      return enable(message);
    }
    else if (method == "disable")
    {
      return disable(message);
    }

    return CdpResponse::error(message.id, -32601, "DOM." + method + " is not supported in content process");
  }

  string CdpDomDomain::getDocument(const CdpMessage &message)
  {
    rapidjson::Document result;
    result.SetObject();
    auto &allocator = result.GetAllocator();

    try
    {
      // Get the current document (for now, create a minimal mock document)
      auto document = getCurrentDocument();
      if (document)
      {
        CdpNode rootNode = convertNodeToCdpNode(document, 2);
        result.AddMember("root", cdpNodeToJson(rootNode, allocator), allocator);
      }
      else
      {
        // Create a minimal mock document when no document is available
        CdpNode mockRoot;
        mockRoot.nodeId = getNextNodeId();
        mockRoot.parentId = 0;
        mockRoot.backendNodeId = mockRoot.nodeId;
        mockRoot.nodeType = static_cast<int>(dom::NodeType::DOCUMENT_NODE);
        mockRoot.nodeName = "#document";
        mockRoot.localName = "";
        mockRoot.nodeValue = "";
        mockRoot.childNodeCount = 1;
        mockRoot.documentURL = "about:blank";
        mockRoot.baseURL = "about:blank";

        // Create mock HTML element
        CdpNode htmlNode;
        htmlNode.nodeId = getNextNodeId();
        htmlNode.parentId = mockRoot.nodeId;
        htmlNode.backendNodeId = htmlNode.nodeId;
        htmlNode.nodeType = static_cast<int>(dom::NodeType::ELEMENT_NODE);
        htmlNode.nodeName = "HTML";
        htmlNode.localName = "html";
        htmlNode.nodeValue = "";
        htmlNode.childNodeCount = 0;

        mockRoot.children.push_back(htmlNode);

        result.AddMember("root", cdpNodeToJson(mockRoot, allocator), allocator);
      }
    }
    catch (const std::exception &e)
    {
      return CdpResponse::error(message.id, -32603, "Internal error: " + string(e.what()));
    }

    return CdpResponse::success(message.id, result);
  }

  string CdpDomDomain::enable(const CdpMessage &message)
  {
    enabled_ = true;

    rapidjson::Document result;
    result.SetObject();
    return CdpResponse::success(message.id, result);
  }

  string CdpDomDomain::disable(const CdpMessage &message)
  {
    enabled_ = false;
    nodeMap_.clear();
    nextNodeId_ = 1;

    rapidjson::Document result;
    result.SetObject();
    return CdpResponse::success(message.id, result);
  }

  CdpNode CdpDomDomain::convertNodeToCdpNode(shared_ptr<dom::Node> node, int depth)
  {
    CdpNode cdpNode;

    if (!node)
    {
      return cdpNode;
    }

    // Assign unique node ID
    cdpNode.nodeId = getNextNodeId();
    cdpNode.backendNodeId = cdpNode.nodeId;

    // Store the mapping for future reference
    nodeMap_[cdpNode.nodeId] = node;

    // Basic node properties
    cdpNode.nodeType = static_cast<int>(node->nodeType);
    cdpNode.nodeName = node->nodeName;
    cdpNode.nodeValue = node->nodeValue().value_or("");

    // Get parent ID if available
    auto parent = node->getParentNode();
    cdpNode.parentId = 0; // Will be set by parent when processing children

    // Get child nodes
    auto children = node->getChildNodes();
    cdpNode.childNodeCount = static_cast<int>(children.size());

    // Convert children if depth allows
    if (depth > 0)
    {
      for (auto child : children)
      {
        CdpNode childNode = convertNodeToCdpNode(child, depth - 1);
        childNode.parentId = cdpNode.nodeId;
        cdpNode.children.push_back(childNode);
      }
    }

    // Handle element-specific properties
    if (node->nodeType == dom::NodeType::ELEMENT_NODE)
    {
      auto element = dynamic_pointer_cast<dom::Element>(node);
      if (element)
      {
        cdpNode.localName = element->localName;

        // Get attributes
        auto attrs = element->getAttributeNames();
        for (const auto &attrName : attrs)
        {
          auto attrValue = element->getAttribute(attrName);
          cdpNode.attributes.push_back({attrName, attrValue});
        }
      }
    }

    // Handle document-specific properties
    if (node->nodeType == dom::NodeType::DOCUMENT_NODE)
    {
      auto document = dynamic_pointer_cast<dom::Document>(node);
      if (document)
      {
        cdpNode.documentURL = document->documentURI();
        cdpNode.baseURL = document->baseURI;
      }
    }

    return cdpNode;
  }

  rapidjson::Value CdpDomDomain::cdpNodeToJson(const CdpNode &node, rapidjson::Document::AllocatorType &allocator)
  {
    rapidjson::Value nodeObj(rapidjson::kObjectType);

    nodeObj.AddMember("nodeId", node.nodeId, allocator);
    nodeObj.AddMember("parentId", node.parentId, allocator);
    nodeObj.AddMember("backendNodeId", node.backendNodeId, allocator);
    nodeObj.AddMember("nodeType", node.nodeType, allocator);
    nodeObj.AddMember("nodeName", rapidjson::Value(node.nodeName.c_str(), allocator), allocator);
    nodeObj.AddMember("localName", rapidjson::Value(node.localName.c_str(), allocator), allocator);
    nodeObj.AddMember("nodeValue", rapidjson::Value(node.nodeValue.c_str(), allocator), allocator);
    nodeObj.AddMember("childNodeCount", node.childNodeCount, allocator);

    // Add children array
    rapidjson::Value childrenArray(rapidjson::kArrayType);
    for (const auto &child : node.children)
    {
      childrenArray.PushBack(cdpNodeToJson(child, allocator), allocator);
    }
    nodeObj.AddMember("children", childrenArray, allocator);

    // Add attributes array
    rapidjson::Value attributesArray(rapidjson::kArrayType);
    for (const auto &attr : node.attributes)
    {
      attributesArray.PushBack(rapidjson::Value(attr.first.c_str(), allocator), allocator);
      attributesArray.PushBack(rapidjson::Value(attr.second.c_str(), allocator), allocator);
    }
    nodeObj.AddMember("attributes", attributesArray, allocator);

    // Add document-specific properties if available
    if (!node.documentURL.empty())
    {
      nodeObj.AddMember("documentURL", rapidjson::Value(node.documentURL.c_str(), allocator), allocator);
    }
    if (!node.baseURL.empty())
    {
      nodeObj.AddMember("baseURL", rapidjson::Value(node.baseURL.c_str(), allocator), allocator);
    }

    return nodeObj;
  }

  int CdpDomDomain::getNextNodeId()
  {
    return nextNodeId_++;
  }

  shared_ptr<dom::Document> CdpDomDomain::getCurrentDocument()
  {
    return TrClientContextPerProcess::Get()->window->document();
  }
}
