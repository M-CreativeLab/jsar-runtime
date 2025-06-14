#include "./keyframe_effect.hpp"
#include "./keyframes.hpp"

namespace dom
{
  using namespace std;

  KeyframeEffect::KeyframeEffect(shared_ptr<dom::Element> target,
                                 optional<Keyframes> keyframes,
                                 const KeyframeEffectOptions options)
      : AnimationEffect()
      , target_(target)
      , keyframes_(make_unique<Keyframes>(*keyframes))
      , composite_(options.composite.value_or(CompositeReplace))
      , iteration_composite_(options.iterationComposite.value_or(CompositeReplace))
      , pseudo_element_str_(options.pseudoElement)
  {
  }

  Keyframes KeyframeEffect::getKeyframes() const
  {
    return *keyframes_;
  }

  void KeyframeEffect::setKeyframes(optional<Keyframes> keyframes)
  {
    if (keyframes.has_value())
      keyframes_ = make_unique<Keyframes>(*keyframes);
    else
      keyframes_->empty();
  }
}
