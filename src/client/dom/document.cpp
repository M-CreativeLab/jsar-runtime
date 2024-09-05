#include "./element.hpp"
#include "./document.hpp"

namespace dom
{
  Document::Document(string contentType)
      : Node(),
        contentType(contentType)
  {
    docInternal = std::make_shared<pugi::xml_document>();
  }

  Document::Document(Document &other)
      : Node(other),
        contentType(other.contentType),
        docInternal(other.docInternal)
  {
  }

  shared_ptr<Element> Document::getElementById(const string &id)
  {
    auto node = docInternal->document_element().find_child_by_attribute("id", id.c_str());
    if (node.empty())
      return nullptr;
    return Element::CreateElement(node);
  }

  XMLDocument::XMLDocument(const std::string &source, bool _autoConnect) : Document("text/xml")
  {
    auto r = docInternal->load_string(source.c_str());
    if (r.status != pugi::xml_parse_status::status_ok)
      throw std::runtime_error("Failed to parse XML document: " + std::string(r.description()));
    resetInternal(docInternal.get());
  }

  HTMLDocument::HTMLDocument(const std::string &source, bool autoConnect) : Document("text/html")
  {
    auto r = docInternal->load_string(source.c_str());
    if (r.status != pugi::xml_parse_status::status_ok)
      throw std::runtime_error("Failed to parse HTML document: " + std::string(r.description()));
    resetInternal(docInternal.get());

    if (autoConnect == true)
      connect();
  }
}
