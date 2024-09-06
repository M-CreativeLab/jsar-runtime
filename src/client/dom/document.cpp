#include <iostream>
#include "./element.hpp"
#include "./document.hpp"
#include "./rendering_context.hpp"

namespace dom
{
  Document::Document(string contentType, bool autoConnect)
      : Node(),
        contentType(contentType),
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

  void Document::setSource(const string &source)
  {
    auto r = docInternal->load_string(source.c_str());
    if (r.status != pugi::xml_parse_status::status_ok)
      throw std::runtime_error("Failed to parse XML document: " + std::string(r.description()));
    resetInternal(docInternal.get(), getPtr<Document>());
  }

  void Document::open(shared_ptr<DocumentRenderingContext> renderingContextToSet)
  {
    renderingContext = renderingContextToSet;
    if (autoConnect)
    {
      connect();
      load();
    }
  }

  shared_ptr<Element> Document::getElementById(const string &id)
  {
    // auto node = docInternal->document_element().find_child_by_attribute("id", id.c_str());
    // if (node.empty())
    //   return nullptr;
    // return Element::CreateElement(node);
    return nullptr;
  }

  XMLDocument::XMLDocument(bool autoConnect) : Document("text/xml", autoConnect)
  {
  }

  HTMLDocument::HTMLDocument(bool autoConnect) : Document("text/html", autoConnect)
  {
  }
}
