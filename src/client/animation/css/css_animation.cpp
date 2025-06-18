#include "./css_animation.hpp"

namespace dom
{
  using namespace std;

  void CSSAnimation::clearOwningElement()
  {
    owning_element_.reset();
  }

  shared_ptr<Element> CSSAnimation::owningElement() const
  {
    if (owning_element_.expired())
      return nullptr;
    return owning_element_.lock();
  }
}
