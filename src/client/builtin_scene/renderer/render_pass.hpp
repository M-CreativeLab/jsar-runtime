#pragma once

#include <string>
#include <ostream>

namespace endor
{
  namespace builtin_scene
  {
    enum class RenderPass
    {
      kOpaques,
      kTransparents,
      kPostProcessing,
    };

    inline std::ostream &operator<<(std::ostream &os, RenderPass pass)
    {
      switch (pass)
      {
      case RenderPass::kOpaques:
        return os << "RenderPass(Opaques)";
      case RenderPass::kTransparents:
        return os << "RenderPass(Transparents)";
      case RenderPass::kPostProcessing:
        return os << "RenderPass(PostProcessing)";
      default:
        return os << "RenderPass(Unknown)";
      }
    }
  }
} // namespace endor
