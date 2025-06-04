#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <client/cssom/properties.hpp>
#include <client/cssom/computed_style.hpp>
#include <client/animation/animation.hpp>
#include <client/animation/css/css_animation.hpp>
#include <client/animation/css/css_transition.hpp>

namespace dom
{
  // A map of animatable properties.
  class AnimatableProperties : public std::vector<std::string>
  {
    using Base = std::unordered_map<client_cssom::NonCustomPropertyId, std::string>;

  public:
    static AnimatableProperties FromTransitionProperty(
        const client_cssom::values::computed::TransitionProperty &property)
    {
      return AnimatableProperties{{property.toCss()}};
    }
  };

  class CSSAnimations final
  {
  public:
    // Represents animations specified via CSS `animation-*` properties.
    class RunningAnimation
    {
    public:
      RunningAnimation(std::shared_ptr<Animation> animation,
                       std::string name)
          : animation(animation),
            name(name)
      {
      }

    public:
      std::shared_ptr<Animation> animation;
      std::string name;
    };

    // Represents transitions specified via CSS `transition-*` properties.
    class RunningTransition
    {
    public:
      RunningTransition(std::shared_ptr<CSSTransition> animation,
                        const AnimatableProperties properties)
          : animation(animation),
            properties(properties)
      {
      }

    public:
      std::shared_ptr<CSSTransition> animation;
      const AnimatableProperties properties;
    };

  public:
    CSSAnimations();
    ~CSSAnimations() = default;

  public:
    bool isEmpty() const
    {
      return running_animations_.empty() &&
             transitions_.empty();
    }

    const std::vector<std::shared_ptr<RunningAnimation>> &runningAnimations() const { return running_animations_; }
    const std::optional<RunningTransition> transition(const std::string &name) const
    {
      auto it = transitions_.find(name);
      if (it != transitions_.end())
        return *it->second;
      return std::nullopt;
    }
    size_t setTransitions(client_cssom::ComputedStyle &);

  private:
    std::vector<std::shared_ptr<RunningAnimation>> running_animations_;
    std::unordered_map<std::string, std::shared_ptr<RunningTransition>> transitions_;
  };
}
