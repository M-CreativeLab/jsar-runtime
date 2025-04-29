#include "./keyframe_effect.hpp"
#include "./keyframes.hpp"

namespace dom
{
  using namespace std;

  KeyframeEffect::KeyframeEffect(std::shared_ptr<dom::Element> target,
                                 const Keyframes &keyframes,
                                 const KeyframeEffectOptions)
  {
  }

  KeyframeEffect::KeyframeEffect(const KeyframeEffect &other)
      : AnimationEffect(other),
        composite_(other.composite_),
        iteration_composite_(other.iteration_composite_),
        target_(other.target_),
        pseudo_element_str_(other.pseudo_element_str_)
  {
  }

  Keyframes KeyframeEffect::getKeyframes() const
  {
    return {};
  }

  void KeyframeEffect::setKeyframes(const Keyframes &keyframes)
  {
  }
}
