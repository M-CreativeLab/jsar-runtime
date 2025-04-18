#include "./hit_test_ray.hpp"

namespace client_layout
{
  using namespace std;

  HitTestRay::HitTestRay(const collision::TrRay &ray)
      : collision::TrRay(ray)
  {
  }

  bool HitTestRay::operator==(const HitTestRay &other) const
  {
    return glm::all(glm::equal(this->origin, other.origin)) &&
           glm::all(glm::equal(this->direction, other.direction));
  }
}
