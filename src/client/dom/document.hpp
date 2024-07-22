#pragma once

#include "./node.hpp"

namespace dom
{
  class Document : public Node
  {
  };

  class XMLDocument : public Document
  {
  };

  class HTMLDocument : public Document
  {
  };
}
