// Copyright 2018 The Dawn & Tint Authors
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <unordered_set>
#include <atomic>

#include <idgen.hpp>
#include <common/command_buffers/gpu/gpu_constants.hpp>

namespace commandbuffers
{
  using GPUBindingNumber = uint32_t;
  constexpr GPUBindingNumber kMaxBindingsPerBindGroupTyped = GPUBindingNumber(gpu_constants::kMaxBindingsPerBindGroup);

  using GPUClientBindingIndex = uint32_t;
  using GPUBindingIndex = uint32_t;

  using GPUBindGroupIndex = uint32_t;
  constexpr GPUBindGroupIndex kMaxBindGroupsTyped = GPUBindGroupIndex(gpu_constants::kMaxBindGroups);

  enum class GPUHandleType : uint32_t
  {
    kAdapter,
    kBindGroup,
    kBindGroupLayout,
    kBuffer,
    kCommandBuffer,
    kCommandEncoder,
    kComputePassEncoder,
    kComputePipeline,
    kDevice,
    kExternalTexture,
    kInstance,
    kPipelineLayout,
    kQuerySet,
    kQueue,
    kRenderBundle,
    kRenderBundleEncoder,
    kRenderPassEncoder,
    kRenderPipeline,
    kSampler,
    kShaderModule,
    kSharedBufferMemory,
    kSharedFence,
    kSharedTextureMemory,
    kSurface,
    kTexelBufferView,
    kTexture,
    kTextureView,

    // Additional internal object types. Keep kExtraObjectTypes in sync when updating.
    kBindGroupLayoutInternal,
  };

  enum class GPUBackendType : uint32_t
  {
    kUndefined,
    kNull,
    kWebGPU,
    kD3D11,
    kD3D12,
    kMetal,
    kVulkan,
    kOpenGL,
    kOpenGLES,
  };

  enum class GPUAdapterType : uint32_t
  {
    kDiscreteGPU,
    kIntegratedGPU,
    kCPU,
    kUnknown,
  };

  enum class GPUShaderStage : uint64_t
  {
    kNone = 0x0000000000000000,
    kVertex = 0x0000000000000001,
    kFragment = 0x0000000000000002,
    kCompute = 0x0000000000000004,
  };

  enum class GPUMapMode : uint64_t
  {
    kNone = 0x0000000000000000,
    kRead = 0x0000000000000001,
    kWrite = 0x0000000000000002,
  };

  enum class GPUBufferBindingType : uint32_t
  {
    kBindingNotUsed,
    kUndefined,
    kUniform,
    kStorage,
    kReadOnlyStorage,
  };

  enum class GPUSamplerBindingType : uint32_t
  {
    kBindingNotUsed,
    kUndefined,
    kFiltering,
    kNonFiltering,
    kComparison,
  };

  enum class GPUTextureSampleType : uint32_t
  {
    kBindingNotUsed,
    kUndefined,
    kFloat,
    kUnfilterableFloat,
    kDepth,
    kSint,
    kUint,
  };

  enum class GPUTextureViewDimension : uint32_t
  {
    kUndefined,
    k1D,
    k2D,
    k2DArray,
    kCube,
    kCubeArray,
    k3D,
  };

  enum class GPUIndexFormat
  {
    kUint16,
    kUint32,
  };

  enum class GPUTextureFormat : uint32_t
  {
    kUndefined,
    kR8Unorm,
    kR8Snorm,
    kR8Uint,
    kR8Sint,
    kR16Unorm,
    kR16Snorm,
    kR16Uint,
    kR16Sint,
    kR16Float,
    kRG8Unorm,
    kRG8Snorm,
    kRG8Uint,
    kRG8Sint,
    kR32Float,
    kR32Uint,
    kR32Sint,
    kRG16Unorm,
    kRG16Snorm,
    kRG16Uint,
    kRG16Sint,
    kRG16Float,
    kRGBA8Unorm,
    kRGBA8UnormSrgb,
    kRGBA8Snorm,
    kRGBA8Uint,
    kRGBA8Sint,
    kBGRA8Unorm,
    kBGRA8UnormSrgb,
    kRGB10A2Uint,
    kRGB10A2Unorm,
    kRG11B10Ufloat,
    kRGB9E5Ufloat,
    kRG32Float,
    kRG32Uint,
    kRG32Sint,
    kRGBA16Unorm,
    kRGBA16Snorm,
    kRGBA16Uint,
    kRGBA16Sint,
    kRGBA16Float,
    kRGBA32Float,
    kRGBA32Uint,
    kRGBA32Sint,
    kStencil8,
    kDepth16Unorm,
    kDepth24Plus,
    kDepth24PlusStencil8,
    kDepth32Float,
    kDepth32FloatStencil8,
    kBC1RGBAUnorm,
    kBC1RGBAUnormSrgb,
    kBC2RGBAUnorm,
    kBC2RGBAUnormSrgb,
    kBC3RGBAUnorm,
    kBC3RGBAUnormSrgb,
    kBC4RUnorm,
    kBC4RSnorm,
    kBC5RGUnorm,
    kBC5RGSnorm,
    kBC6HRGBUfloat,
    kBC6HRGBFloat,
    kBC7RGBAUnorm,
    kBC7RGBAUnormSrgb,
    kETC2RGB8Unorm,
    kETC2RGB8UnormSrgb,
    kETC2RGB8A1Unorm,
    kETC2RGB8A1UnormSrgb,
    kETC2RGBA8Unorm,
    kETC2RGBA8UnormSrgb,
    kEACR11Unorm,
    kEACR11Snorm,
    kEACRG11Unorm,
    kEACRG11Snorm,
    kASTC4x4Unorm,
    kASTC4x4UnormSrgb,
    kASTC5x4Unorm,
    kASTC5x4UnormSrgb,
    kASTC5x5Unorm,
    kASTC5x5UnormSrgb,
    kASTC6x5Unorm,
    kASTC6x5UnormSrgb,
    kASTC6x6Unorm,
    kASTC6x6UnormSrgb,
    kASTC8x5Unorm,
    kASTC8x5UnormSrgb,
    kASTC8x6Unorm,
    kASTC8x6UnormSrgb,
    kASTC8x8Unorm,
    kASTC8x8UnormSrgb,
    kASTC10x5Unorm,
    kASTC10x5UnormSrgb,
    kASTC10x6Unorm,
    kASTC10x6UnormSrgb,
    kASTC10x8Unorm,
    kASTC10x8UnormSrgb,
    kASTC10x10Unorm,
    kASTC10x10UnormSrgb,
    kASTC12x10Unorm,
    kASTC12x10UnormSrgb,
    kASTC12x12Unorm,
    kASTC12x12UnormSrgb,
    kR8BG8Biplanar420Unorm,
    kR10X6BG10X6Biplanar420Unorm,
    kR8BG8A8Triplanar420Unorm,
    kR8BG8Biplanar422Unorm,
    kR8BG8Biplanar444Unorm,
    kR10X6BG10X6Biplanar422Unorm,
    kR10X6BG10X6Biplanar444Unorm,
    kExternal,
  };

  enum class GPUStorageTextureAccess : uint32_t
  {
    kBindingNotUsed,
    kUndefined,
    kWriteOnly,
    kReadOnly,
    kReadWrite,
  };

  enum class GPUDynamicBindingKind : uint32_t
  {
    kUndefined,
    kSampledTexture,
  };

  enum class GPUFeatureName : uint32_t
  {
    kCoreFeaturesAndLimits,
    kDepthClipControl,
  };

  enum class GPUFeatureLevel : uint32_t
  {
    kUndefined = 0x00000000,
    kCompatibility = 0x00000001,
    kCore = 0x00000002,
  };

  enum class GPUPowerPreference : uint32_t
  {
    Undefined = 0x00000000,
    LowPower = 0x00000001,
    HighPerformance = 0x00000002,
  };

  class GPUSupportedFeatures : public std::unordered_set<GPUFeatureName>
  {
    using std::unordered_set<GPUFeatureName>::unordered_set;
  };

  struct GPUSupportedLimits
  {
    uint32_t maxTextureDimension1D = 8192;
    uint32_t maxTextureDimension2D = 8192;
    uint32_t maxTextureDimension3D = 2048;
    uint32_t maxTextureArrayLayers = 2048;
    uint32_t maxBindGroups = 4;
    uint32_t maxBindingsPerBindGroup = 640;
    uint32_t maxDynamicUniformBuffersPerPipelineLayout = 8;
    uint32_t maxDynamicStorageBuffersPerPipelineLayout = 4;
    uint32_t maxSampledTexturesPerShaderStage = 16;
    uint32_t maxSamplersPerShaderStage = 16;
    uint32_t maxStorageBuffersPerShaderStage = 8;
    uint32_t maxStorageTexturesPerShaderStage = 8;
    uint32_t maxUniformBuffersPerShaderStage = 12;
    uint64_t maxUniformBufferBindingSize = 65536;
    uint64_t maxStorageBufferBindingSize = 134217728;
    uint32_t minUniformBufferOffsetAlignment = 256;
    uint32_t minStorageBufferOffsetAlignment = 256;
    uint32_t maxVertexBuffers = 8;
    uint64_t maxBufferSize = 4294967296;
    uint32_t maxVertexAttributes = 16;
    uint32_t maxVertexBufferArrayStride = 2048;
    uint32_t maxInterStageShaderVariables = 16;
    uint32_t maxColorAttachments = 8;
    uint32_t maxColorAttachmentBytesPerSample = 32;
    uint32_t maxComputeWorkgroupStorageSize = 16384;
    uint32_t maxComputeInvocationsPerWorkgroup = 256;
    uint32_t maxComputeWorkgroupSizeX = 256;
    uint32_t maxComputeWorkgroupSizeY = 256;
    uint32_t maxComputeWorkgroupSizeZ = 64;
    uint32_t maxComputeWorkgroupsPerDimension = 65535;

    bool operator==(const GPUSupportedLimits &rhs) const;
  };

  class ErrorMonad
  {
  public:
    struct ErrorTag
    {
    };
    static constexpr ErrorTag kError = {};

    struct DelayedInitializationTag
    {
    };
    static constexpr DelayedInitializationTag kDelayedInitialization = {};

    ErrorMonad();
    explicit ErrorMonad(ErrorTag tag);
    explicit ErrorMonad(DelayedInitializationTag tag);
    virtual ~ErrorMonad() = default;

    // Test if the error state is valid yet. It is an error to check the error state before the
    // object is initialized.
    bool initialized() const;

    bool isError() const;

  protected:
    void setInitializedError();
    void setInitializedNoError();

  private:
    std::atomic<uint64_t> tag_payload_;
    uint64_t fetchAnd(uint64_t arg);

    static constexpr uint64_t kNotInitializedPayload = 0b11;
    static constexpr uint64_t kInitializedErrorPayload = 0b00;
    static constexpr uint64_t kInitializedNoErrorPayload = 0b10;

    static constexpr uint64_t kInitializedMask = 0b1;
    static constexpr uint64_t kInitialized = 0b0;
  };

  class GPUDeviceBase;
  class GPUInstanceBase;

  class GPUObject : public ErrorMonad
  {
  public:
    explicit GPUObject(std::shared_ptr<GPUDeviceBase> device);
    GPUObject(std::shared_ptr<GPUDeviceBase> device, ErrorTag tag);
    GPUObject(std::shared_ptr<GPUDeviceBase> device, DelayedInitializationTag tag);

    GPUInstanceBase *instance() const;
    std::shared_ptr<GPUDeviceBase> device() const;

  private:
    std::shared_ptr<GPUDeviceBase> device_;
  };

  typedef uint32_t GPUIdentifier;
  class GPUHandle : public GPUObject
  {
  private:
    static inline TrIdGeneratorBase<GPUIdentifier> Ids = TrIdGeneratorBase<GPUIdentifier>(0);

  public:
    struct LabelNotImplementedTag
    {
    };
    static constexpr LabelNotImplementedTag kLabelNotImplemented = {};
    struct UntrackedByDeviceTag
    {
    };
    static constexpr UntrackedByDeviceTag kUntrackedByDevice = {};

    GPUHandle(std::shared_ptr<GPUDeviceBase> device, std::string_view label);
    GPUHandle(std::shared_ptr<GPUDeviceBase> device, ErrorTag tag, std::string_view label = {});
    GPUHandle(std::shared_ptr<GPUDeviceBase> device, DelayedInitializationTag tag, std::string_view label = {});
    GPUHandle(std::shared_ptr<GPUDeviceBase> device, LabelNotImplementedTag tag);

    virtual GPUHandleType type() const = 0;
    void setLabel(std::string label);
    const std::string &getLabel() const;

  private:
    std::string label_ = "";
    const GPUIdentifier id_;
  };
}
