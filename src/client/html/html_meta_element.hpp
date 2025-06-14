#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
  class HTMLMetaElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLMetaElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("META", ownerDocument)
    {
    }

  public:
    string getContent()
    {
      return getAttribute("content");
    }
    void setContent(string value)
    {
      setAttribute("content", value);
    }
    string getHttpEquiv()
    {
      return getAttribute("http-equiv");
    }
    void setHttpEquiv(string value)
    {
      setAttribute("http-equiv", value);
    }
    string getMedia()
    {
      return getAttribute("media");
    }
    void setMedia(string value)
    {
      setAttribute("media", value);
    }
    string getName()
    {
      return getAttribute("name");
    }
    void setName(string value)
    {
      setAttribute("name", value);
    }
  };
}
