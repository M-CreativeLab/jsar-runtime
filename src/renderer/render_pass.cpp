#include "./render_pass.hpp"

namespace renderer
{
  TrRenderPass::TrRenderPass(RenderPassType type, const std::string &name)
      : type_(type)
      , name_(name)
      , isActive_(false)
      , commandBuffers_()
      , gpuRenderPassEncoder_(nullptr)
  {
  }

  TrRenderPass::~TrRenderPass()
  {
    // Note: We don't delete command buffers here as ownership may be elsewhere.
    // The caller should use clearAndDeleteCommandBuffers() if needed.
    commandBuffers_.clear();
  }

  void TrRenderPass::addCommandBuffer(commandbuffers::TrCommandBufferBase *commandBuffer)
  {
    if (commandBuffer != nullptr)
    {
      commandBuffers_.push_back(commandBuffer);
    }
  }

  void TrRenderPass::clearCommandBuffers()
  {
    commandBuffers_.clear();
  }

  void TrRenderPass::clearAndDeleteCommandBuffers()
  {
    for (auto *cb : commandBuffers_)
    {
      if (cb != nullptr)
      {
        delete cb;
      }
    }
    commandBuffers_.clear();
  }

  void TrRenderPass::setGPURenderPassEncoder(Ref<commandbuffers::GPURenderPassEncoder> encoder)
  {
    gpuRenderPassEncoder_ = encoder;
  }

  void TrRenderPass::begin()
  {
    isActive_ = true;
  }

  void TrRenderPass::end()
  {
    isActive_ = false;
  }

  // TrRenderPassCollection implementation

  TrRenderPassCollection::TrRenderPassCollection()
      : opaquePass_(std::make_unique<TrRenderPass>(RenderPassType::kOpaque, "OpaqueRenderPass"))
      , transparentPass_(std::make_unique<TrRenderPass>(RenderPassType::kTransparent, "TransparentRenderPass"))
      , offscreenPass_(std::make_unique<TrRenderPass>(RenderPassType::kOffscreen, "OffscreenRenderPass"))
  {
  }

  TrRenderPassCollection::~TrRenderPassCollection()
  {
    // The unique_ptrs will clean up the render passes automatically.
  }

  TrRenderPass *TrRenderPassCollection::getOrCreatePass(RenderPassType type)
  {
    switch (type)
    {
    case RenderPassType::kOpaque:
      return opaquePass_.get();
    case RenderPassType::kTransparent:
      return transparentPass_.get();
    case RenderPassType::kOffscreen:
      return offscreenPass_.get();
    default:
      return opaquePass_.get(); // Default to opaque
    }
  }

  void TrRenderPassCollection::clearAllCommandBuffers()
  {
    if (opaquePass_)
      opaquePass_->clearCommandBuffers();
    if (transparentPass_)
      transparentPass_->clearCommandBuffers();
    if (offscreenPass_)
      offscreenPass_->clearCommandBuffers();
  }

  void TrRenderPassCollection::clearAndDeleteAllCommandBuffers()
  {
    if (opaquePass_)
      opaquePass_->clearAndDeleteCommandBuffers();
    if (transparentPass_)
      transparentPass_->clearAndDeleteCommandBuffers();
    if (offscreenPass_)
      offscreenPass_->clearAndDeleteCommandBuffers();
  }

  void TrRenderPassCollection::resetForNewFrame()
  {
    // End any active passes and clear command buffers
    if (opaquePass_ && opaquePass_->isActive())
      opaquePass_->end();
    if (transparentPass_ && transparentPass_->isActive())
      transparentPass_->end();
    if (offscreenPass_ && offscreenPass_->isActive())
      offscreenPass_->end();

    clearAllCommandBuffers();
  }
}
