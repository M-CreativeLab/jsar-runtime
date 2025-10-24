#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_section_element.hpp>
#include "./html_element.hpp"

namespace endor
{
  namespace script_bindings
  {
    namespace html_bindings
    {
      class HTMLSectionElement;
      using HTMLSectionElementBase = scripting_base::ObjectWrap<HTMLSectionElement,
                                                                dom::HTMLSectionElement,
                                                                HTMLElement>;

      class HTMLSectionElement : public HTMLSectionElementBase
      {
        using HTMLSectionElementBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "HTMLElement";
        }
      };
    }
  }
} // namespace endor
