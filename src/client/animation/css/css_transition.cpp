#include "./css_transition.hpp"

namespace endor
{
  namespace dom
  {
    using namespace std;

    void CSSTransition::clearOwningElement()
    {
      owning_element_.reset();
    }

    shared_ptr<Element> CSSTransition::owningElement() const
    {
      if (owning_element_.expired())
        return nullptr;
      return owning_element_.lock();
    }
  }
} // namespace endor
