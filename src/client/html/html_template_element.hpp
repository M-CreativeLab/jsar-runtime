#pragma once

#include <memory>
#include <client/dom/document_fragment.hpp>

#include "./html_element.hpp"

namespace dom
{
  class HTMLTemplateElement final : public HTMLElement
  {
    using HTMLElement::HTMLElement;
    friend class Element;

  public:
    enum class ShadowRootMode
    {
      Open,
      Closed
    };

  public:
    inline const DocumentFragment &content() const { return *content_; }
    inline std::shared_ptr<DocumentFragment> getContent() { return content_; }

  private:
    inline DocumentFragment &content() { return *content_; }

  private:
    void createdCallback(bool from_scripting) override;

  private:
    std::shared_ptr<DocumentFragment> content_;
    ShadowRootMode shadowRootMode_;
    bool shadowRootDelegatesFocus_;
    bool shadowRootClonable_;
    bool shadowRootSerializable_;
  };
}
