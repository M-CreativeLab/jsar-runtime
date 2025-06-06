#include <client/dom/document.hpp>
#include "./html_template_element.hpp"

namespace dom
{
  void HTMLTemplateElement::createdCallback(bool from_scripting)
  {
    HTMLElement::createdCallback(from_scripting);

    content_ = getOwnerDocumentChecked().createDocumentFragment();
    shadowRootMode_ = ShadowRootMode::Open;
    shadowRootDelegatesFocus_ = false;
    shadowRootClonable_ = false;
    shadowRootSerializable_ = false;
  }
}
