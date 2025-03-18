#include "./html_template_element.hpp"
#include "./document.hpp"

namespace dom
{
  void HTMLTemplateElement::createdCallback()
  {
    HTMLElement::createdCallback();

    content_ = getOwnerDocumentChecked().createDocumentFragment();
    shadowRootMode_ = ShadowRootMode::Open;
    shadowRootDelegatesFocus_ = false;
    shadowRootClonable_ = false;
    shadowRootSerializable_ = false;
  }
}
