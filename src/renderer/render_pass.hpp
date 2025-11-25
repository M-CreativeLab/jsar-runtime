#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <string>
#include <common/command_buffers/base.hpp>
#include <common/command_buffers/gpu/gpu_renderpass_encoder.hpp>

namespace renderer
{
  /**
   * The type of render pass to categorize command buffers.
   */
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

  /**
   * Convert RenderPassType to a human-readable string.
   */
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

  class TrContentRenderer;

  /**
   * The `TrRenderPass` class represents a render pass that manages a collection of command buffers.
   * It wraps the `GPURenderPassEncoder` and provides an interface for adding and executing command buffers
   * based on the pass type (opaque, transparent, or offscreen).
   */
  class TrRenderPass final
  {
  public:
    /**
     * Create a new render pass.
     *
     * @param type The type of render pass.
     * @param name The name of the render pass for debugging purposes.
     */
    TrRenderPass(RenderPassType type, const std::string &name);
    ~TrRenderPass();

    /**
     * Get the type of this render pass.
     */
    inline RenderPassType getType() const
    {
      return type_;
    }

    /**
     * Get the name of this render pass.
     */
    inline const std::string &getName() const
    {
      return name_;
    }

    /**
     * Add a command buffer to this render pass.
     *
     * Note: This class does NOT take ownership of the command buffer.
     * The caller is responsible for managing the command buffer's lifecycle.
     * Use `clearAndDeleteCommandBuffers()` if the render pass should delete
     * the command buffers when clearing.
     *
     * @param commandBuffer The command buffer to add.
     */
    void addCommandBuffer(commandbuffers::TrCommandBufferBase *commandBuffer);

    /**
     * Get the command buffers in this render pass.
     *
     * @returns A reference to the vector of command buffers.
     */
    inline std::vector<commandbuffers::TrCommandBufferBase *> &getCommandBuffers()
    {
      return commandBuffers_;
    }

    /**
     * Get the command buffers in this render pass (const version).
     *
     * @returns A const reference to the vector of command buffers.
     */
    inline const std::vector<commandbuffers::TrCommandBufferBase *> &getCommandBuffers() const
    {
      return commandBuffers_;
    }

    /**
     * Check if this render pass has any command buffers.
     *
     * @returns `true` if there are command buffers, `false` otherwise.
     */
    inline bool hasCommandBuffers() const
    {
      return !commandBuffers_.empty();
    }

    /**
     * Get the number of command buffers in this render pass.
     *
     * @returns The count of command buffers.
     */
    inline size_t getCommandBuffersCount() const
    {
      return commandBuffers_.size();
    }

    /**
     * Clear all command buffers from this render pass.
     * Note: This does not delete the command buffers; the caller is responsible for memory management.
     */
    void clearCommandBuffers();

    /**
     * Clear and delete all command buffers from this render pass.
     * This will delete the command buffer pointers and clear the vector.
     */
    void clearAndDeleteCommandBuffers();

    /**
     * Set the associated GPU render pass encoder for this render pass.
     * This is used when integrating with the WebGPU-style rendering pipeline.
     *
     * @param encoder The GPU render pass encoder.
     */
    void setGPURenderPassEncoder(Ref<commandbuffers::GPURenderPassEncoder> encoder);

    /**
     * Get the associated GPU render pass encoder.
     *
     * @returns The GPU render pass encoder, or nullptr if not set.
     */
    inline Ref<commandbuffers::GPURenderPassEncoder> getGPURenderPassEncoder() const
    {
      return gpuRenderPassEncoder_;
    }

    /**
     * Check if this render pass has an associated GPU render pass encoder.
     *
     * @returns `true` if an encoder is set, `false` otherwise.
     */
    inline bool hasGPURenderPassEncoder() const
    {
      return gpuRenderPassEncoder_ != nullptr;
    }

    /**
     * Begin this render pass.
     * This should be called before adding command buffers to prepare the pass.
     */
    void begin();

    /**
     * End this render pass.
     * This should be called after all command buffers have been executed.
     */
    void end();

    /**
     * Check if this render pass is currently active (between begin and end).
     *
     * @returns `true` if the pass is active, `false` otherwise.
     */
    inline bool isActive() const
    {
      return isActive_;
    }

  private:
    RenderPassType type_;
    std::string name_;
    bool isActive_ = false;
    std::vector<commandbuffers::TrCommandBufferBase *> commandBuffers_;
    Ref<commandbuffers::GPURenderPassEncoder> gpuRenderPassEncoder_ = nullptr;
  };

  /**
   * A collection of render passes for a content renderer.
   * This manages the opaque, transparent, and offscreen render passes.
   */
  class TrRenderPassCollection final
  {
  public:
    TrRenderPassCollection();
    ~TrRenderPassCollection();

    /**
     * Get or create a render pass of the specified type.
     *
     * @param type The type of render pass.
     * @returns A pointer to the render pass.
     */
    TrRenderPass *getOrCreatePass(RenderPassType type);

    /**
     * Get the opaque render pass.
     *
     * @returns A pointer to the opaque render pass.
     */
    inline TrRenderPass *getOpaquePass()
    {
      return opaquePass_.get();
    }

    /**
     * Get the transparent render pass.
     *
     * @returns A pointer to the transparent render pass.
     */
    inline TrRenderPass *getTransparentPass()
    {
      return transparentPass_.get();
    }

    /**
     * Get the offscreen render pass.
     *
     * @returns A pointer to the offscreen render pass.
     */
    inline TrRenderPass *getOffscreenPass()
    {
      return offscreenPass_.get();
    }

    /**
     * Clear all command buffers from all render passes.
     * Note: This does not delete the command buffers.
     */
    void clearAllCommandBuffers();

    /**
     * Clear and delete all command buffers from all render passes.
     */
    void clearAndDeleteAllCommandBuffers();

    /**
     * Reset all render passes for a new frame.
     */
    void resetForNewFrame();

  private:
    std::unique_ptr<TrRenderPass> opaquePass_;
    std::unique_ptr<TrRenderPass> transparentPass_;
    std::unique_ptr<TrRenderPass> offscreenPass_;
  };
}
