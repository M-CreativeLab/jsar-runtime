#include "./animatable.hpp"

namespace dom
{
  using namespace std;

  Animation &Animatable::animate(Keyframes &keyframes)
  {
    throw runtime_error("Not implemented");
  }

  vector<shared_ptr<Animation>> Animatable::getAnimations(optional<GetAnimationsOptions> options)
  {
    return {};
  }
}
