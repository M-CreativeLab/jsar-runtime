#pragma once

#include <ostream>

namespace client_layout
{
  enum class HitTestPhase : uint8_t
  {
    kSelfBlockBackground,
    kDescendantBlockBackgrounds,
    kFloat,
    kForeground,
  };

  inline std::ostream &operator<<(std::ostream &os, HitTestPhase phase)
  {
    switch (phase)
    {
    case HitTestPhase::kSelfBlockBackground:
      os << "kSelfBlockBackground";
      break;
    case HitTestPhase::kDescendantBlockBackgrounds:
      os << "kDescendantBlockBackgrounds";
      break;
    case HitTestPhase::kFloat:
      os << "kFloat";
      break;
    case HitTestPhase::kForeground:
      os << "kForeground";
      break;
    default:
      os << "Unknown Phase";
      break;
    }
    return os;
  }
}
