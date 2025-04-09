#pragma once

namespace client_layout
{
  enum class HitTestPhase
  {
    kSelfBlockBackground,
    kDescendantBlockBackgrounds,
    kFloat,
    kForeground,
  };
}
