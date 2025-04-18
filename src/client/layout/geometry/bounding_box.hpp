#pragma once

#include <ostream>
#include <common/collision/culling/bounding_box.hpp>

namespace client_layout::geometry
{
  class BoundingBox : public collision::culling::TrBoundingBox
  {
    using collision::culling::TrBoundingBox::TrBoundingBox;

  public:
    void move(const glm::vec3 &offset)
    {
      translate(offset.x, offset.y, offset.z);
    }

    friend std::ostream &operator<<(std::ostream &os, const BoundingBox &bbox)
    {
      auto min = bbox.minimumWorld;
      auto max = bbox.maximumWorld;
      auto center = bbox.centerWorld;
      auto size = bbox.extendSizeWorld;

      os << "BoundingBox("
         << "min=[" << min.x << ", " << min.y << ", " << min.z << "], "
         << "max=[" << max.x << ", " << max.y << ", " << max.z << "], "
         << "center=[" << center.x << ", " << center.y << ", " << center.z << "], "
         << "size=[" << size.x << ", " << size.y << ", " << size.z << "]"
         << ")";
      return os;
    }
  };
}
