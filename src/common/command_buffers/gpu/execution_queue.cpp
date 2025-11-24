#include <common/command_buffers/gpu/execution_queue.hpp>

namespace commandbuffers::gpu
{
  bool ExecutionQueueBase::hasScheduledCommands() const
  {
    return false;
  }
}
