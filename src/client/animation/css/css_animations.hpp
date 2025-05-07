#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <client/animation/animation.hpp>
#include <client/cssom/computed_style.hpp>

namespace dom
{
  class CSSAnimations final
  {
  public:
    CSSAnimations();
    ~CSSAnimations() = default;

  public:
    bool isEmpty() const
    {
      return running_animations_.empty() &&
             transitions_.empty();
    }

  private:
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

    class RunningTransition
    {
    public:
      RunningTransition(std::shared_ptr<Animation> animation,
                        std::shared_ptr<const client_cssom::ComputedStyle> from,
                        std::shared_ptr<const client_cssom::ComputedStyle> to)
          : animation(animation),
            from(from),
            to(to)
      {
      }

    public:
      std::shared_ptr<Animation> animation;
      std::shared_ptr<const client_cssom::ComputedStyle> from;
      std::shared_ptr<const client_cssom::ComputedStyle> to;
    };

    std::vector<std::shared_ptr<RunningAnimation>> running_animations_;

    using TransitionMap = std::unordered_map<std::string, std::shared_ptr<RunningTransition>>;
    TransitionMap transitions_;
  };
}
