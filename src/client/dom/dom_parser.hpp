#pragma once

#include <string>
#include <memory>
#include <client/scripting_base/v8_object_holder.hpp>
#include "./document.hpp"

namespace endor
{
  namespace dom
  {
    enum class DOMParsingType
    {
      XML,
      HTML,
      XSML,
      SVG,
    };

    class DOMParser : public scripting_base::JSObjectHolder
    {
    public:
      DOMParser();
      ~DOMParser() = default;

      template <typename DocumentType>
      shared_ptr<DocumentType> parseFromString(const std::string &source, DOMParsingType _type)
      {
        auto document = make_shared<DocumentType>(nullptr, false);
        document->setSource(source);
        return document;
      }
    };
  }
} // namespace endor
