#include "./computed_style.hpp"

namespace client_cssom
{
  ComputedStyle::ComputedStyle(const CSSStyleDeclaration &style)
      : std::unordered_map<std::string, std::string>()
  {
    resolve(style);
  }

  void ComputedStyle::resolve(const CSSStyleDeclaration &style)
  {
    for (int index = 0; index < style.length(); index++)
    {
      auto propertyName = style.item(index);
      auto value = style.getPropertyValue(propertyName);
      insert({propertyName, value});

      // TODO: resolve the property value
    }
  }
}
