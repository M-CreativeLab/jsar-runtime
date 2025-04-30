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
      std::optional<float> delay;
      std::optional<float> duration;
      std::optional<float> endDelay;

      std::optional<size_t> iterations;
      std::optional<float> iterationStart;

      std::optional<AnimationEffect::FillMode> fill;
      std::optional<AnimationEffect::Direction> direction;
      std::unique_ptr<TimingFunction> easing = nullptr;

      std::optional<Composite> composite;
      std::optional<Composite> iterationComposite;
      std::optional<std::string> pseudoElement;
    };

    KeyframeEffect(std::shared_ptr<dom::Element> target, std::optional<Keyframes>, const KeyframeEffectOptions);

  public:
    Keyframes getKeyframes() const;
    void setKeyframes(std::optional<Keyframes>);

  private:
    Composite composite_ = CompositeReplace;
    Composite iteration_composite_ = CompositeReplace;
    std::weak_ptr<dom::Element> target_;
    std::optional<std::string> pseudo_element_str_;
    std::unique_ptr<Keyframes> keyframes_;
  };
}
