#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <common/command_buffers/base.hpp>
#include <common/command_buffers/shared.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>
#include <common/command_buffers/gpu/gpu_command_buffer.hpp>
#include <common/xr/types.hpp>

namespace renderer
{
  /**
   * EncodedPass represents a WebGPU command buffer that has been encoded and is ready for submission.
   * 
   * According to the WebGPU specification (https://www.w3.org/TR/webgpu/#command-buffers):
   * - Command buffers are pre-recorded lists of GPU commands that can be submitted to a queue
   * - They are created by finishing a GPUCommandEncoder
   * - They can be submitted multiple times without re-encoding
   * 
   * This structure stores:
   * - The encoded WebGPU command buffer
   * - XR rendering information for pass routing (sessionId, stereoId, viewIndex)
   * - Metadata about the pass type (framebuffer-dependent, resource-creating, etc.)
   */
  struct EncodedPass
  {
    // The encoded WebGPU command buffer
    // TODO: Consider using shared_ptr if GPU handles are non-copyable
    std::unique_ptr<commandbuffers::GPUCommandBufferBase> commandBuffer;

    // XR rendering information for routing this pass to the correct host renderpass
    // Valid when isXRPass is true
    xr::TrXRFrameRenderingInfo renderingInfo;

    // Metadata flags
    bool isFramebufferDependent = false;
    bool isResourceCreating = false;
    bool isXRPass = false;

    EncodedPass() = default;
    EncodedPass(std::unique_ptr<commandbuffers::GPUCommandBufferBase> cmd,
                const xr::TrXRFrameRenderingInfo &info,
                bool isFbDependent,
                bool isResourceCreate,
                bool isXR)
        : commandBuffer(std::move(cmd))
        , renderingInfo(info)
        , isFramebufferDependent(isFbDependent)
        , isResourceCreating(isResourceCreate)
        , isXRPass(isXR)
    {
    }

    // Move constructor
    EncodedPass(EncodedPass &&other) noexcept
        : commandBuffer(std::move(other.commandBuffer))
        , renderingInfo(other.renderingInfo)
        , isFramebufferDependent(other.isFramebufferDependent)
        , isResourceCreating(other.isResourceCreating)
        , isXRPass(other.isXRPass)
    {
    }

    // Move assignment
    EncodedPass &operator=(EncodedPass &&other) noexcept
    {
      if (this != &other)
      {
        commandBuffer = std::move(other.commandBuffer);
        renderingInfo = other.renderingInfo;
        isFramebufferDependent = other.isFramebufferDependent;
        isResourceCreating = other.isResourceCreating;
        isXRPass = other.isXRPass;
      }
      return *this;
    }

    // Delete copy operations to ensure unique ownership
    EncodedPass(const EncodedPass &) = delete;
    EncodedPass &operator=(const EncodedPass &) = delete;
  };

  /**
   * TrCommandBufferManager manages the encoding and storage of command buffers for the renderer.
   * 
   * ## Purpose
   * This class accepts TrCommandBufferBase instances from the runtime, encodes them into 
   * WebGPU command buffers using GPUDeviceBase, and stores them as EncodedPasses for later 
   * submission by the host renderer.
   * 
   * ## WebGPU Specification Alignment
   * The implementation follows these WebGPU spec sections:
   * - Command Encoding: https://www.w3.org/TR/webgpu/#command-encoding
   * - Command Buffers: https://www.w3.org/TR/webgpu/#command-buffers
   * - Render Passes: https://www.w3.org/TR/webgpu/#render-passes
   * - Resource Synchronization: https://www.w3.org/TR/webgpu/#synchronization
   * 
   * ## Design Rationale
   * WebGPU command buffers can be:
   * 1. Recorded ahead of time
   * 2. Submitted multiple times
   * 3. Reused across frames for static scenes
   * 
   * This allows the host to schedule rendering more flexibly and optimize performance.
   * 
   * ## Thread Safety
   * All public methods are thread-safe. Internal storage is protected by std::mutex.
   * 
   * ## Usage Pattern
   * ```cpp
   * auto manager = std::make_unique<TrCommandBufferManager>(device);
   * 
   * // Add commands (thread-safe)
   * manager->addCommandBuffer(std::move(cmd1));
   * manager->addCommandBuffer(std::move(cmd2));
   * 
   * // In render loop
   * auto passes = manager->getEncodedPasses();
   * for (auto& pass : passes) {
   *   if (pass.isXRPass) {
   *     host->submitToXRRenderPass(pass.commandBuffer.get(), pass.renderingInfo);
   *   } else {
   *     host->submitToMainRenderPass(pass.commandBuffer.get());
   *   }
   * }
   * manager->clearEncodedPasses();
   * ```
   */
  class TrCommandBufferManager
  {
  public:
    /**
     * Construct a command buffer manager with a GPU device.
     * 
     * @param device The WebGPU device to use for command encoding. Must not be null.
     *               The device must remain valid for the lifetime of this manager.
     */
    explicit TrCommandBufferManager(commandbuffers::GPUDeviceBase *device);

    /**
     * Destructor - ensures proper cleanup of encoded passes.
     */
    ~TrCommandBufferManager();

    // Delete copy operations to prevent accidental copies
    TrCommandBufferManager(const TrCommandBufferManager &) = delete;
    TrCommandBufferManager &operator=(const TrCommandBufferManager &) = delete;

    /**
     * Add a command buffer for encoding.
     * 
     * This method:
     * 1. Inspects cmd->type to determine the dispatch path
     * 2. Uses CommandTypes helpers to classify the command
     * 3. Encodes the command into a WebGPU command buffer
     * 4. Enqueues the encoded pass for later submission
     * 
     * Thread-safe: Can be called from multiple threads simultaneously.
     * 
     * @param cmd The command buffer to encode. Ownership is transferred.
     * 
     * ## Implementation Status
     * TODO: Implement actual encoding logic that dispatches based on command type:
     *   - IsFramebufferDependentCommand -> encode into render pass
     *   - IsResourceCreatingCommand -> encode resource creation
     *   - IsXRFrameControl -> handle XR frame lifecycle
     *   - Generic commands -> encode into compute/copy pass as appropriate
     * 
     * TODO: Reference WebGPU spec sections for:
     *   - Render pass encoding: https://www.w3.org/TR/webgpu/#render-passes
     *   - Compute pass encoding: https://www.w3.org/TR/webgpu/#compute-passes
     *   - Resource creation: https://www.w3.org/TR/webgpu/#resource-creation
     */
    void addCommandBuffer(std::unique_ptr<commandbuffers::TrCommandBufferBase> cmd);

    /**
     * Get all encoded passes for submission.
     * 
     * Returns a const reference to avoid copying large command buffers.
     * The returned reference is valid until clearEncodedPasses() is called.
     * 
     * Thread-safe: Acquires read lock on internal storage.
     * 
     * @return A vector of encoded passes ready for submission
     */
    const std::vector<EncodedPass> &getEncodedPasses() const;

    /**
     * Clear all stored encoded passes.
     * 
     * This should be called after the host has submitted all passes.
     * According to WebGPU spec, command buffers can be released after submission completes.
     * 
     * Thread-safe: Acquires write lock on internal storage.
     * 
     * TODO: Consider adding a callback mechanism to notify when GPU has finished executing
     *       the command buffers, to support proper resource lifecycle management.
     */
    void clearEncodedPasses();

    /**
     * Get the number of currently stored encoded passes.
     * 
     * Thread-safe.
     * 
     * @return The count of encoded passes
     */
    size_t getEncodedPassCount() const;

  private:
    /**
     * Encode a framebuffer-dependent command.
     * 
     * These commands modify or depend on framebuffer state and should be encoded
     * into render passes according to WebGPU spec.
     * 
     * @param cmd The command to encode
     * @return The encoded pass, or nullptr if encoding failed
     * 
     * TODO: Implement render pass encoding:
     *   1. Create GPUCommandEncoder via device_->createCommandEncoder()
     *   2. Begin render pass with appropriate descriptor
     *   3. Encode draw calls, state changes, etc.
     *   4. End render pass
     *   5. Finish encoder to create command buffer
     */
    std::unique_ptr<EncodedPass> encodeFramebufferCommand(
      const std::unique_ptr<commandbuffers::TrCommandBufferBase> &cmd);

    /**
     * Encode a resource-creating command.
     * 
     * These commands create GPU resources (buffers, textures, shaders, etc.)
     * and may not need to be encoded into command buffers at all - they can
     * be executed immediately on the device.
     * 
     * @param cmd The command to encode
     * @return The encoded pass, or nullptr if encoding failed
     * 
     * TODO: Implement resource creation:
     *   - Buffer creation: device_->createBuffer()
     *   - Texture creation: device_->createTexture() (via impl)
     *   - Shader module creation: device_->createShaderModule()
     *   - Pipeline creation: device_->createRenderPipeline() (via impl)
     * 
     * FIXME: Determine if resource commands need to be encoded as passes
     *        or can be executed synchronously. WebGPU spec allows both patterns.
     */
    std::unique_ptr<EncodedPass> encodeResourceCommand(
      const std::unique_ptr<commandbuffers::TrCommandBufferBase> &cmd);

    /**
     * Encode a generic command that doesn't fit other categories.
     * 
     * @param cmd The command to encode
     * @return The encoded pass, or nullptr if encoding failed
     * 
     * TODO: Implement generic command encoding:
     *   - Copy operations: encode into copy pass
     *   - Query operations: handle synchronously
     *   - State queries: execute immediately
     */
    std::unique_ptr<EncodedPass> encodeGenericCommand(
      const std::unique_ptr<commandbuffers::TrCommandBufferBase> &cmd);

  private:
    // The GPU device used for command encoding
    // Not owned by this class - must remain valid for the lifetime of the manager
    commandbuffers::GPUDeviceBase *device_;

    // Storage for encoded passes
    // Protected by mutex for thread-safety
    mutable std::mutex passes_mutex_;
    std::vector<EncodedPass> encoded_passes_;
  };

} // namespace renderer
