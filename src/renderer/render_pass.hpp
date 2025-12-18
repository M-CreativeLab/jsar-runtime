#pragma once

#include <string>
#include <memory>
#include <optional>

#include <common/utility.hpp>
#include <common/command_buffers/base.hpp>
#include <common/command_buffers/gpu/gpu_command_buffer.hpp>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>
#include <common/command_buffers/gpu/gpu_renderpass_encoder.hpp>

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
    TrRenderPass(RenderPassType type, const std::string &name);
    ~TrRenderPass();

    RenderPassType type() const;
    const std::string &name() const;
    bool isActive() const;
    Ref<commandbuffers::GPURenderPassEncoder> encoder() const;

    void receiveIncomingRequest(const commandbuffers::TrCommandBufferRequest &request);
    void begin();
    void end();
    std::unique_ptr<commandbuffers::GPUCommandBufferBase> finish(
      std::optional<std::string> label = std::nullopt);

  private:
    RenderPassType type_;
    std::string name_;
    bool active_;
    Ref<commandbuffers::GPUCommandEncoder> command_encoder_;
    Ref<commandbuffers::GPURenderPassEncoder> renderpass_encoder_;
    commandbuffers::GPURenderPassDescriptor renderpass_descriptor_;
  };
}
