#pragma once

#include <optional>
#include <string>
#include <memory>
#include <client/dom/types.hpp>

#include "./animation_effect.hpp"

namespace dom
{
  class Keyframes;
  class KeyframeEffect : public AnimationEffect
  {
  public:
    enum Composite
    {
      CompositeReplace,
      CompositeAdd,
      CompositeAccumulate
    };
    struct KeyframeEffectOptions
    {
      float delay;
      float duration;
    };

    KeyframeEffect(std::shared_ptr<dom::Element> target, const Keyframes &keyframes, const KeyframeEffectOptions);
    KeyframeEffect(const KeyframeEffect&);

  public:
    Keyframes getKeyframes() const;
    void setKeyframes(const Keyframes &);

  private:
    Composite composite_ = CompositeReplace;
    Composite iteration_composite_ = CompositeReplace;
    std::weak_ptr<dom::Element> target_;
    std::optional<std::string> pseudo_element_str_;
  };
}
