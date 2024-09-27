#include <iostream>
#include "./element.hpp"
#include "./document.hpp"
#include "./rendering_context.hpp"

namespace dom
{
  Document::Document(string contentType, shared_ptr<DocumentRenderingContext> renderingContext, bool autoConnect)
      : Node(NodeType::DOCUMENT_NODE, "#document", std::nullopt),
        contentType(contentType),
        renderingContext(renderingContext),
        autoConnect(autoConnect)
  {
    docInternal = std::make_shared<pugi::xml_document>();
  }

  Document::Document(Document &other)
      : Node(other),
        contentType(other.contentType),
        docInternal(other.docInternal)
  {
  }

  void Document::setUrl(const string &url)
  {
    if (renderingContext == nullptr)
      throw std::runtime_error("DocumentRenderingContext is not set");

    if (url.find_first_of("http:") == 0 ||
        url.find_first_of("https:") == 0 ||
        url.find_first_of("file:") == 0)
      baseURI = url;
    else
      baseURI = "http://example.com";

    renderingContext->fetchTextSourceResource(url, [this](const string &source)
                                              { setSource(source); });
  }

  void Document::setSource(const string &source)
  {
    auto r = docInternal->load_string(source.c_str());
    if (r.status != pugi::xml_parse_status::status_ok)
      throw std::runtime_error("Failed to parse XML document: " + std::string(r.description()));

    resetFrom(docInternal, getPtr<Document>());
    isSourceLoaded = true;

    if (shouldOpen)
      openInternal();
  }

  void Document::open()
  {
    shouldOpen = true;
    if (isSourceLoaded)
      openInternal();
  }

  shared_ptr<Element> Document::getElementById(const string &id)
  {
    // auto node = docInternal->document_element().find_child_by_attribute("id", id.c_str());
    // if (node.empty())
    //   return nullptr;
    // return Element::CreateElement(node);
    return nullptr;
  }

  void Document::openInternal()
  {
    if (autoConnect)
    {
      connect();
      load();
    }
  }

  XMLDocument::XMLDocument(shared_ptr<DocumentRenderingContext> renderingContext, bool autoConnect)
      : Document("text/xml", renderingContext, autoConnect)
  {
  }

  HTMLDocument::HTMLDocument(shared_ptr<DocumentRenderingContext> renderingContext, bool autoConnect)
      : Document("text/html", renderingContext, autoConnect)
  {
  }
}
