#include <common/command_buffers/gpu/gpu_per_stage.hpp>

using namespace std;
using namespace commandbuffers::gpu_constants;

namespace commandbuffers
{
  // static
  bitset<gpu_constants::kNumStages> GPUStages::IterateStages(GPUShaderStage stages)
  {
    bitset<gpu_constants::kNumStages> bits(static_cast<uint32_t>(stages));
    return bits;
  }

  // static
  GPUShaderStage GPUStages::StageBit(GPUSingleShaderStage stage)
  {
    assert(static_cast<uint32_t>(stage) < kNumStages);
    return static_cast<GPUShaderStage>(1 << static_cast<uint32_t>(stage));
  }
}
