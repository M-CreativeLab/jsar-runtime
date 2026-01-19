#pragma once

namespace commandbuffers::gpu
{
  class SyncScopeUsageTracker
  {
  };

  class ComputePassResourceUsageTracker
  {
  };

  class RenderPassResourceUsageTracker : public SyncScopeUsageTracker
  {
  };
}
