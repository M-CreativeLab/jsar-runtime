#include <renderer/render_pass.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  TrRenderPass::TrRenderPass(RenderPassType type, const string &name)
      : type_(type)
      , name_(name)
      , active_(false)
      , command_encoder_(nullptr)
      , renderpass_encoder_(nullptr)
  {
  }

  TrRenderPass::~TrRenderPass()
  {
  }

  RenderPassType TrRenderPass::type() const
  {
    return type_;
  }

  const string &TrRenderPass::name() const
  {
    return name_;
  }

  bool TrRenderPass::isActive() const
  {
    return active_;
  }

  Ref<GPURenderPassEncoder> TrRenderPass::encoder() const
  {
    return renderpass_encoder_;
  }

  void TrRenderPass::clearAttachments(bool clearColor,
                                      bool clearDepth,
                                      bool clearStencil,
                                      const float rgba[4],
                                      float depthValue,
                                      int stencilValue)
  {
    (void)clearColor;
    (void)clearDepth;
    (void)clearStencil;
    (void)rgba;
    (void)depthValue;
    (void)stencilValue;

    if (!renderpass_descriptor_.colorAttachments.empty())
    {
      auto &color = renderpass_descriptor_.colorAttachments[0];
      color.clearColor[0] = rgba[0];
      color.clearColor[1] = rgba[1];
      color.clearColor[2] = rgba[2];
      color.clearColor[3] = rgba[3];
      color.loadOp = GPURenderPassDescriptor::LoadOp::Clear;
    }

    if (renderpass_descriptor_.depthStencilAttachment.has_value())
    {
      auto &ds = *renderpass_descriptor_.depthStencilAttachment;
      if (clearDepth)
      {
        ds.depthClearValue = depthValue;
        ds.depthLoadOp = GPURenderPassDescriptor::LoadOp::Clear;
      }
      if (clearStencil)
      {
        ds.stencilClearValue = static_cast<float>(stencilValue);
        ds.stencilLoadOp = GPURenderPassDescriptor::LoadOp::Clear;
      }
    }
  }

  void TrRenderPass::setColorAttachmentCount(size_t n)
  {
    renderpass_descriptor_.colorAttachments.resize(n);
  }

  void TrRenderPass::ensureDepthStencilAttachment()
  {
    if (!renderpass_descriptor_.depthStencilAttachment.has_value())
      renderpass_descriptor_.depthStencilAttachment = GPURenderPassDescriptor::DepthStencilAttachment{};
  }

  void TrRenderPass::receiveIncomingRequest(const TrCommandBufferRequest &request)
  {
  }

  void TrRenderPass::begin()
  {
    active_ = true;
    {
      renderpass_encoder_ = command_encoder_->beginRenderPass(renderpass_descriptor_);
    }
  }

  void TrRenderPass::end()
  {
    active_ = false;
    renderpass_encoder_->end();
  }

  unique_ptr<GPUCommandBufferBase> TrRenderPass::finish(optional<string> label)
  {
    return command_encoder_->finish(label);
  }
}
