#include <optional>
#include <client/macros.h>
#include "./css_style_declaration.hpp"

namespace client_cssom
{
  using namespace std;
  
  bool CSSStyleDeclaration::equals(const CSSStyleDeclaration &other) const
  {
    if (length() != other.length()) // quick check if the length is different
      return false;
    return cssText() == other.cssText(); // TODO: support more quick check?
  }

  bool CSSStyleDeclaration::update(const CSSStyleDeclaration &other, bool omitIfPresent)
  {
    bool isChanged = false;
    for (size_t i = 0; i < other.length(); i++)
    {
      auto name = other.item(i);
      if (hasProperty(name))
      {
        bool isSelfPropImportant = getPropertyPriority(name) == CSSPropertyPriority::Important;
        bool isOtherPropImportant = other.getPropertyPriority(name) == CSSPropertyPriority::Important;
        if (isSelfPropImportant && isOtherPropImportant)
        {
          isSelfPropImportant = false;
          isOtherPropImportant = true;
        }

        // Omit this property if it is already present, or if the other one is not important
        if (omitIfPresent && !isOtherPropImportant)
          continue;

        // Skip the property if the self one is important
        if (isSelfPropImportant)
          continue;

        auto value = other.getPropertyValue(name);
        if (getPropertyValue(name) != value)
        {
          setProperty(name, value,
                      isOtherPropImportant
                          ? CSSPropertyPriority::Important
                          : CSSPropertyPriority::Normal);
          isChanged = true;
        }
      }
      else
      {
        setProperty(name, other.getPropertyValue(name), other.getPropertyPriority(name));
        isChanged = true;
      }
    }
    return isChanged;
  }

  bool CSSStyleDeclaration::setCssText(const std::string &cssText)
  {
    auto pdb = PropertyDeclarationBlock::ParseStyleDeclaration(cssText);
    if (pdb == nullptr)
      return false;
    pdb_ = std::move(pdb);
    cachedCssText_ = pdb_->cssText();

    // Notify the property changed callback if it is set
    if (propertyChangedCallback_)
      propertyChangedCallback_(cachedCssText_.value());
    return true;
  }
}
