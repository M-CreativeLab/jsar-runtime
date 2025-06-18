#pragma once

#include <client/animation/animation.hpp>

namespace dom
{
  class Element;
  class CSSAnimation : public Animation
  {
  public:
    bool isCSSAnimation() const override
    {
      return true;
    }

    void clearOwningElement() override;
    std::shared_ptr<Element> owningElement() const override;

  private:
    std::weak_ptr<Element> owning_element_;
  };
}