#include <client/cssom/units.hpp>
#include "./attr.hpp"
#include "./html_element.hpp"
#include "./document-inl.hpp"

namespace dom
{
  using namespace std;
  using LayoutAllocator = crates::layout2::Allocator;
  using LayoutNode = crates::layout2::Node;

  string DashStyleToCamelCase(const string &dashStyle)
  {
    string camelCase;
    bool upper = false;
    for (auto c : dashStyle)
    {
      if (c == '-')
      {
        upper = true;
      }
      else
      {
        if (upper)
        {
          camelCase += toupper(c);
          upper = false;
        }
        else
        {
          camelCase += c;
        }
      }
    }
    return camelCase;
  }

  string CamelCaseToDashStyle(const string &camelCase)
  {
    string dashStyle;
    for (auto c : camelCase)
    {
      if (isupper(c))
      {
        if (!dashStyle.empty())
          dashStyle += '-';
        dashStyle += tolower(c);
      }
      else
      {
        dashStyle += c;
      }
    }
    return dashStyle;
  }

  // TODO: Implement the following methods.
  void HTMLElement::blur() {}
  void HTMLElement::focus() {}
  void HTMLElement::click() {}

  optional<string> HTMLElement::getDataset(const string &key)
  {
    auto it = dataset_.find(key);
    if (it != dataset_.end())
      return it->second;
    return nullopt;
  }

  void HTMLElement::setDataset(const string &key, const string &value)
  {
    dataset_[key] = value;
    setAttribute("data-" + CamelCaseToDashStyle(key), value);
  }

  void HTMLElement::removeDataset(const string &key)
  {
    dataset_.erase(key);
    removeAttribute("data-" + CamelCaseToDashStyle(key));
  }

  void HTMLElement::createdCallback()
  {
    Element::createdCallback();

    // Update dataset from the attributes.
    for (auto &attr : attributeNodes_)
    {
      if (attr.first.size() > 5 && attr.first.substr(0, 5) == "data-")
      {
        string key = DashStyleToCamelCase(attr.first.substr(5));
        string value = attr.second->value;
        dataset_[key] = value;
      }
    }

    // Configure the style property change callback.
    auto onPropertyChanged = [this](const string &name)
    {
      // reset the style cache
      auto document = getOwnerDocumentReference();
      if (document != nullptr)
        document->styleCache().resetStyle(getPtr<HTMLElement>());
    };
    // Create style declaration from the default style & the style attribute.
    style_ = make_shared<client_cssom::CSSStyleDeclaration>(getAttribute("style"));
    style_->setPropertyChangedCallback(onPropertyChanged);
  }
}
