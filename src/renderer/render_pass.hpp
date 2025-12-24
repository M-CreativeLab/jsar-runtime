#pragma once

#include <string>
#include <memory>
#include <optional>
#include <unordered_map>
#include <array>

#include <common/utility.hpp>
#include <common/command_buffers/base.hpp>
#include <common/command_buffers/gpu/gpu_command_buffer.hpp>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>
#include <common/command_buffers/gpu/gpu_renderpass_encoder.hpp>
#include <command_buffers/shared.hpp>
#include <common/xr/common.hpp>
// forward declare to avoid heavy include in header
namespace xr
{
  class Device;
}

namespace renderer
{
  enum class RenderPassType
  {
    /**
     * Opaque render pass for rendering opaque objects.
     * This is the default pass for objects without transparency.
     */
    kOpaque,
    /**
     * Transparent render pass for rendering transparent objects.
     * Objects with blending enabled should be routed here.
     */
    kTransparent,
    /**
     * Offscreen render pass for rendering to off-screen targets.
     * Used when the framebuffer is different from the main render target.
     */
    kOffscreen,
  };

  inline const char *RenderPassTypeToString(RenderPassType type)
  {
    switch (type)
    {
    case RenderPassType::kOpaque:
      return "Opaque";
    case RenderPassType::kTransparent:
      return "Transparent";
    case RenderPassType::kOffscreen:
      return "Offscreen";
    default:
      return "Unknown";
    }
  }

  /**
   * The `TrRenderPass` class represents a render pass that manages a collection of command buffers.
   * It wraps the `GPURenderPassEncoder` and provides an interface for adding and executing command buffers
   * based on the pass type (opaque, transparent, or offscreen).
   */
  class TrRenderPass final
  {
  public:
    TrRenderPass(RenderPassType type,
                 const std::string &name,
                 Ref<commandbuffers::GPUDeviceBase> device,
                 xr::Device *xrDevice);
    ~TrRenderPass();

    RenderPassType type() const;
    const std::string &name() const;
    bool isActive() const;
    Ref<commandbuffers::GPURenderPassEncoder> encoder() const;
    Ref<commandbuffers::GPURenderPassEncoder> encoder(int eyeIndex) const;
    void bindTarget(WebGLuint framebuffer);
    void bindTarget(WebGLuint framebuffer, int eyeIndex);
    void discardTarget(WebGLuint framebuffer);
    void discardTarget(WebGLuint framebuffer, int eyeIndex);

    void clearAttachments(bool clearColor,
                          bool clearDepth,
                          bool clearStencil,
                          const float rgba[4],
                          float depthValue,
                          int stencilValue);
    void clearAttachments(bool clearColor,
                          bool clearDepth,
                          bool clearStencil,
                          const float rgba[4],
                          float depthValue,
                          int stencilValue,
                          int eyeIndex);

    void setColorAttachmentCount(size_t n);
    void setColorAttachmentCount(size_t n, int eyeIndex);
    void ensureDepthStencilAttachment();
    void ensureDepthStencilAttachment(int eyeIndex);

    void receiveIncomingRequest(const commandbuffers::TrCommandBufferRequest &request);
    void begin();
    void begin(int eyeIndex);
    void end();
    void end(int eyeIndex);
    std::unique_ptr<commandbuffers::GPUCommandBufferBase> finish(
      std::optional<std::string> label = std::nullopt);
    std::unique_ptr<commandbuffers::GPUCommandBufferBase> finish(
      std::optional<std::string> label,
      int eyeIndex);
    void submit(std::optional<std::string> label = std::nullopt);

  private:
    static constexpr int kMaxEyes = 2;
    RenderPassType type_;
    std::string name_;
    bool active_;
    Ref<commandbuffers::GPUDeviceBase> gpu_device_;
    xr::Device *xr_device_ = nullptr;
    std::array<Ref<commandbuffers::GPUCommandEncoder>, kMaxEyes> command_encoders_;
    std::array<std::optional<WebGLuint>, kMaxEyes> active_targets_;
    std::unordered_map<WebGLuint, Ref<commandbuffers::GPURenderPassEncoder>> renderpass_encoders_;
    std::unordered_map<WebGLuint, commandbuffers::GPURenderPassDescriptor> renderpass_descriptors_;
  };
}
