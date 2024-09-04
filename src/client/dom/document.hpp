#pragma once

#include <memory>
#include "./node.hpp"
#include "pugixml/pugixml.hpp"

using namespace std;
using namespace pugi;

namespace dom
{
  class Document : public Node
  {
  public:
    Document();
    Document(Document &other);
    ~Document() = default;

  protected:
    shared_ptr<pugi::xml_document> docInternal;
  };

  class XMLDocument : public Document
  {
  public:
    XMLDocument(const std::string &source);
    ~XMLDocument() = default;
  };

  class HTMLDocument : public Document
  {
  public:
    HTMLDocument(const std::string &source);
    ~HTMLDocument() = default;
  };
}
