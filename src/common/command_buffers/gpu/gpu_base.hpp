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

  struct GPUConstantEntry
  {
    std::string_view key;
    double value;
  };

  struct GPUExtent2D
  {
    uint32_t width;
    uint32_t height;

    // Equality operators, mostly for testing. Note that this tests
    // strict pointer-pointer equality if the struct contains member pointers.
    bool operator==(const GPUExtent2D &rhs) const;
  };

  struct GPUExtent3D
  {
    uint32_t width;
    uint32_t height = 1;
    uint32_t depthOrArrayLayers = 1;

    // Equality operators, mostly for testing. Note that this tests
    // strict pointer-pointer equality if the struct contains member pointers.
    bool operator==(const GPUExtent3D &rhs) const;
  };

  enum class GPUPrimitiveTopology : uint32_t
  {
    kUndefined,
    kPointList,
    kLineList,
    kLineStrip,
    kTriangleList,
    kTriangleStrip,
  };

  enum class GPUIndexFormat : uint32_t
  {
    kUndefined,
    kUint16,
    kUint32,
  };

  enum class GPUFrontFace : uint32_t
  {
    kUndefined,
    kCCW,
    kCW,
  };

  enum class GPUCullMode : uint32_t
  {
    kUndefined,
    kNone,
    kFront,
    kBack,
  };

  enum class GPUVertexStepMode : uint32_t
  {
    kUndefined,
    kVertex,
    kInstance,
  };

  enum class GPUVertexFormat : uint32_t
  {
    kUint8,
    kUint8x2,
    kUint8x4,
    kSint8,
    kSint8x2,
    kSint8x4,
    kUnorm8,
    kUnorm8x2,
    kUnorm8x4,
    kSnorm8,
    kSnorm8x2,
    kSnorm8x4,
    kUint16,
    kUint16x2,
    kUint16x4,
    kSint16,
    kSint16x2,
    kSint16x4,
    kUnorm16,
    kUnorm16x2,
    kUnorm16x4,
    kSnorm16,
    kSnorm16x2,
    kSnorm16x4,
    kFloat16,
    kFloat16x2,
    kFloat16x4,
    kFloat32,
    kFloat32x2,
    kFloat32x3,
    kFloat32x4,
    kUint32,
    kUint32x2,
    kUint32x3,
    kUint32x4,
    kSint32,
    kSint32x2,
    kSint32x3,
    kSint32x4,
    kUnorm10_10_10_2,
    kUnorm8x4BGRA,
  };

  enum class GPUComponentSwizzle : uint32_t
  {
    kUndefined,
    kZero,
    kOne,
    kR,
    kG,
    kB,
    kA,
  };

  enum class GPUCompositeAlphaMode : uint32_t
  {
    kAuto,
    kOpaque,
    kPremultiplied,
    kUnpremultiplied,
    kInherit,
  };

  enum class GPUColorWriteMask : uint64_t
  {
    kNone,
    kRed,
    kGreen,
    kBlue,
    kAlpha,
    kAll,
  };

  enum class GPUBlendFactor : uint32_t
  {
    kUndefined,
    kZero,
    kOne,
    kSrc,
    kOneMinusSrc,
    kSrcAlpha,
    kOneMinusSrcAlpha,
    kDst,
    kOneMinusDst,
    kDstAlpha,
    kOneMinusDstAlpha,
    kSrcAlphaSaturated,
    kConstant,
    kOneMinusConstant,
    kSrc1,
    kOneMinusSrc1,
    kSrc1Alpha,
    kOneMinusSrc1Alpha,
  };

  enum class GPUBlendOperation : uint32_t
  {
    kUndefined,
    kAdd,
    kSubtract,
    kReverseSubtract,
    kMin,
    kMax,
  };

  enum class GPUCompareFunction : uint32_t
  {
    kUndefined,
    kNever,
    kLess,
    kEqual,
    kLessEqual,
    kGreater,
    kNotEqual,
    kGreaterEqual,
    kAlways,
  };

  enum class GPUStencilOperation : uint32_t
  {
    kUndefined,
    kKeep,
    kZero,
    kReplace,
    kInvert,
    kIncrementClamp,
    kDecrementClamp,
    kIncrementWrap,
    kDecrementWrap,
  };

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

  enum class GPUTextureAspect : uint32_t
  {
    kUndefined,
    kAll,
    kStencilOnly,
    kDepthOnly,
    kPlane0Only,
    kPlane1Only,
    kPlane2Only,
  };

  enum class GPUTextureUsage : uint64_t
  {
    kNone,
    kCopySrc,
    kCopyDst,
    kTextureBinding,
    kStorageBinding,
    kRenderAttachment,
    kTransientAttachment,
    kStorageAttachment,
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

  enum class GPUTextureDimension : uint32_t
  {
    kUndefined,
    k1D,
    k2D,
    k3D,
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

  struct GPUTextureComponentSwizzle
  {
    GPUComponentSwizzle r = GPUComponentSwizzle::kUndefined;
    GPUComponentSwizzle g = GPUComponentSwizzle::kUndefined;
    GPUComponentSwizzle b = GPUComponentSwizzle::kUndefined;
    GPUComponentSwizzle a = GPUComponentSwizzle::kUndefined;
  };

  enum class GPUStorageTextureAccess : uint32_t
  {
    kBindingNotUsed,
    kUndefined,
    kWriteOnly,
    kReadOnly,
    kReadWrite,
  };

  struct GPUPrimitiveState
  {
    GPUPrimitiveTopology topology = GPUPrimitiveTopology::kTriangleList;
    GPUIndexFormat stripIndexFormat = GPUIndexFormat::kUndefined;
    GPUFrontFace frontFace = GPUFrontFace::kCCW;
    GPUCullMode cullMode = GPUCullMode::kNone;
    bool unclippedDepth = false;
  };

  struct GPUStencilFaceState
  {
    GPUCompareFunction compare = GPUCompareFunction::kUndefined;
    GPUStencilOperation failOp = GPUStencilOperation::kUndefined;
    GPUStencilOperation depthFailOp = GPUStencilOperation::kUndefined;
    GPUStencilOperation passOp = GPUStencilOperation::kUndefined;
  };

  struct GPUDepthStencilState
  {
    GPUTextureFormat format = GPUTextureFormat::kUndefined;
    std::optional<bool> depthWriteEnabled = std::nullopt;
    GPUCompareFunction depthCompare = GPUCompareFunction::kUndefined;
    GPUStencilFaceState stencilFront;
    GPUStencilFaceState stencilBack;
    uint32_t stencilReadMask = 0xFFFFFFFF;
    uint32_t stencilWriteMask = 0xFFFFFFFF;
    int32_t depthBias = 0;
    float depthBiasSlopeScale = 0.f;
    float depthBiasClamp = 0.f;
  };

  struct GPUMultisampleState
  {
    uint32_t count = 1;
    uint32_t mask = 0xFFFFFFFF;
    bool alphaToCoverageEnabled = false;
  };

  struct GPUBlendComponent
  {
    GPUBlendOperation operation = GPUBlendOperation::kUndefined;
    GPUBlendFactor srcFactor = GPUBlendFactor::kUndefined;
    GPUBlendFactor dstFactor = GPUBlendFactor::kUndefined;
  };

  struct GPUBlendState
  {
    GPUBlendComponent color = {};
    GPUBlendComponent alpha = {};
  };

  struct GPUColorTargetState
  {
    GPUTextureFormat format = GPUTextureFormat::kUndefined;
    GPUBlendState const *blend = nullptr;
    GPUColorWriteMask writeMask = GPUColorWriteMask::kAll;
  };

  struct GPUFragmentState
  {
    // ShaderModuleBase *module;
    std::string_view entryPoint;
    size_t constantCount = 0;
    GPUConstantEntry const *constants = nullptr;
    size_t targetCount;
    GPUColorTargetState const *targets = nullptr;
  };

  struct GPUVertexAttribute
  {
    GPUVertexFormat format = {};
    uint64_t offset;
    uint32_t shaderLocation;
  };

  struct GPUVertexBufferLayout
  {
    GPUVertexStepMode stepMode = GPUVertexStepMode::kUndefined;
    uint64_t arrayStride;
    size_t attributeCount;
    GPUVertexAttribute const *attributes = nullptr;
  };

  struct GPUVertexState
  {
    // ShaderModuleBase *module;
    std::string_view entryPoint;
    size_t constantCount = 0;
    GPUConstantEntry const *constants = nullptr;
    size_t bufferCount = 0;
    GPUVertexBufferLayout const *buffers = nullptr;
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

  class GPUInstance;
  class GPUDeviceBase;

  class GPUObject : public ErrorMonad
  {
  public:
    explicit GPUObject(std::shared_ptr<GPUDeviceBase> device);
    GPUObject(std::shared_ptr<GPUDeviceBase> device, ErrorTag tag);
    GPUObject(std::shared_ptr<GPUDeviceBase> device, DelayedInitializationTag tag);

    GPUInstance *instance() const;
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

  public:
    const GPUIdentifier id;

  private:
    std::string label_ = "";
  };
}
