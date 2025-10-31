#pragma once

#include <unordered_set>
#include <vector>

#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_buffer.hpp>
#include <common/command_buffers/gpu/gpu_texture.hpp>
#include <common/command_buffers/gpu/gpu_bind_group.hpp>

namespace commandbuffers::gpu
{
  // Info about how a buffer is used and in which shader stages
  struct BufferSyncInfo
  {
    GPUBufferUsage usage = GPUBufferUsage::kNone;
    GPUShaderStage shaderStages = GPUShaderStage::kNone;
  };

  struct TextureSyncInfo
  {
    // GPUTextureUsage usage = GPUTextureUsage::None;
    GPUShaderStage shaderStages = GPUShaderStage::kNone;
    bool operator==(const TextureSyncInfo &b) const = default;
  };

  struct SyncScopeResourceUsage
  {
    std::vector<GPUBufferBase *> buffers;
    std::vector<BufferSyncInfo> bufferSyncInfos;

    std::vector<GPUTextureBase *> textures;
    // std::vector<TextureSubresourceSyncInfo> textureSyncInfos;

    // std::vector<GPUExternalTextureBase *> externalTextures;
    std::vector<GPUBindGroupBase *> dynamicBindingArrays;
  };

  struct RenderPassResourceUsage : public SyncScopeResourceUsage
  {
    // Storage to track the occlusion queries used during the pass.
    // std::vector<QuerySetBase*> querySets;
    // std::vector<std::vector<bool>> queryAvailabilities;
  };

  struct ComputePassResourceUsage
  {
    std::vector<SyncScopeResourceUsage> dispatchUsages;

    // All the resources referenced by this compute pass for validation in Queue::Submit.
    std::unordered_set<GPUBufferBase *> referencedBuffers;
    std::unordered_set<GPUTextureBase *> referencedTextures;
    // std::unordered_set<ExternalTextureBase *> referencedExternalTextures;
    std::unordered_set<GPUBindGroupBase *> referencedDynamicBindingArrays;
  };

  using RenderPassUsages = std::vector<RenderPassResourceUsage>;
  using ComputePassUsages = std::vector<ComputePassResourceUsage>;

  struct CommandBufferResourceUsage
  {
    RenderPassUsages renderPasses;
    ComputePassUsages computePasses;

    // Resources used in commands that aren't in a pass.
    std::unordered_set<std::shared_ptr<GPUBufferBase>> topLevelBuffers;
    std::unordered_set<std::shared_ptr<GPUTextureBase>> topLevelTextures;
  };
}
