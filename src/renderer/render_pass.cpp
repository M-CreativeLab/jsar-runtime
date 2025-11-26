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
