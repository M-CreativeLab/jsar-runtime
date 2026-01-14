#include <renderer/render_pass.hpp>
#include <renderer/render_api.hpp>
#include <stdexcept>
#include <xr/device.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  TrRenderPass::TrRenderPass(RenderPassType type,
                             const string &name,
                             Ref<GPUDeviceBase> device,
                             xr::Device *xrDevice)
      : type_(type)
      , name_(name)
      , active_(false)
      , gpu_device_(device)
      , xr_device_(xrDevice)
  {
    size_t viewCount = 1;
    if (type_ == RenderPassType::kOffscreen)
    {
      viewCount = 1;
    }
    else if (xr_device_)
    {
      viewCount = xr_device_->getUsedViewsCount();
    }

    for (int i = 0; i < kMaxEyes; ++i)
    {
      if (i < viewCount)
      {
        GPUCommandEncoderDescriptor desc;
        desc.label = name_;
        Ref<GPUCommandEncoder> command_encoder = GPUCommandEncoder::Create(gpu_device_, desc);
        command_encoders_[i] = command_encoder;

        renderpass_descriptors_[i] = GPURenderPassDescriptor{};
        renderpass_encoders_[i] = command_encoder->beginRenderPass(renderpass_descriptors_[i]);
      }
      else
      {
        command_encoders_[i] = nullptr;
      }
      active_targets_[i] = std::nullopt;
    }
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
    return encoder(0);
  }

  Ref<GPURenderPassEncoder> TrRenderPass::encoder(int eyeIndex) const
  {
    if (eyeIndex < 0 || eyeIndex >= kMaxEyes)
      return nullptr;
    if (!active_targets_[eyeIndex].has_value())
      return nullptr;
    return renderpass_encoders_[eyeIndex];
  }

  void TrRenderPass::bindTarget(WebGLuint framebuffer)
  {
    bindTarget(framebuffer, 0);
  }

  void TrRenderPass::bindTarget(WebGLuint framebuffer, int eyeIndex)
  {
    if (eyeIndex < 0 || eyeIndex >= kMaxEyes)
      return;
    active_targets_[eyeIndex] = framebuffer;
  }

  void TrRenderPass::discardTarget(WebGLuint framebuffer)
  {
    discardTarget(framebuffer, 0);
  }

  void TrRenderPass::discardTarget(WebGLuint framebuffer, int eyeIndex)
  {
    if (eyeIndex >= 0 && eyeIndex < kMaxEyes)
    {
      if (active_targets_[eyeIndex].has_value() && active_targets_[eyeIndex].value() == framebuffer)
      {
        renderpass_encoders_[eyeIndex] = nullptr;
        renderpass_descriptors_[eyeIndex] = GPURenderPassDescriptor{};
        active_targets_[eyeIndex].reset();
      }
    }
  }

  void TrRenderPass::clearAttachments(bool clearColor,
                                      bool clearDepth,
                                      bool clearStencil,
                                      const float rgba[4],
                                      float depthValue,
                                      int stencilValue)
  {
    clearAttachments(clearColor, clearDepth, clearStencil, rgba, depthValue, stencilValue, 0);
  }

  void TrRenderPass::clearAttachments(bool clearColor,
                                      bool clearDepth,
                                      bool clearStencil,
                                      const float rgba[4],
                                      float depthValue,
                                      int stencilValue,
                                      int eyeIndex)
  {
    (void)clearColor;
    (void)clearDepth;
    (void)clearStencil;
    (void)rgba;
    (void)depthValue;
    (void)stencilValue;

    if (eyeIndex < 0 || eyeIndex >= kMaxEyes)
      return;
    if (!active_targets_[eyeIndex].has_value())
      return;
    auto &descriptor = renderpass_descriptors_[eyeIndex];

    if (!descriptor.colorAttachments.empty())
    {
      auto &color = descriptor.colorAttachments[0];
      color.clearColor[0] = rgba[0];
      color.clearColor[1] = rgba[1];
      color.clearColor[2] = rgba[2];
      color.clearColor[3] = rgba[3];
      color.loadOp = GPURenderPassDescriptor::LoadOp::Clear;
    }

    if (descriptor.depthStencilAttachment.has_value())
    {
      auto &ds = *descriptor.depthStencilAttachment;
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
    setColorAttachmentCount(n, 0);
  }

  void TrRenderPass::setColorAttachmentCount(size_t n, int eyeIndex)
  {
    if (eyeIndex < 0 || eyeIndex >= kMaxEyes)
      return;
    if (!active_targets_[eyeIndex].has_value())
      return;
    auto &descriptor = renderpass_descriptors_[eyeIndex];
    descriptor.colorAttachments.resize(n);
  }

  void TrRenderPass::ensureDepthStencilAttachment()
  {
    ensureDepthStencilAttachment(0);
  }

  void TrRenderPass::ensureDepthStencilAttachment(int eyeIndex)
  {
    if (eyeIndex < 0 || eyeIndex >= kMaxEyes)
      return;
    if (!active_targets_[eyeIndex].has_value())
      return;
    auto &descriptor = renderpass_descriptors_[eyeIndex];
    if (!descriptor.depthStencilAttachment.has_value())
      descriptor.depthStencilAttachment = GPURenderPassDescriptor::DepthStencilAttachment{};
  }

  void TrRenderPass::receiveIncomingRequest(const TrCommandBufferRequest &request)
  {
  }

  void TrRenderPass::begin()
  {
    begin(0);
  }

  void TrRenderPass::begin(int eyeIndex)
  {
    active_ = true;
    if (eyeIndex < 0 || eyeIndex >= kMaxEyes)
      return;
    if (!active_targets_[eyeIndex].has_value())
      return;
    
    auto &descriptor = renderpass_descriptors_[eyeIndex];
    if (command_encoders_[eyeIndex] == nullptr)
      return;
    auto enc = command_encoders_[eyeIndex]->beginRenderPass(descriptor);
    renderpass_encoders_[eyeIndex] = enc;
  }

  void TrRenderPass::end()
  {
    end(0);
  }

  void TrRenderPass::end(int eyeIndex)
  {
    active_ = false;
    if (eyeIndex < 0 || eyeIndex >= kMaxEyes)
      return;
    if (!active_targets_[eyeIndex].has_value())
      return;
    
    if (renderpass_encoders_[eyeIndex] != nullptr)
      renderpass_encoders_[eyeIndex]->end();
  }

  unique_ptr<GPUCommandBufferBase> TrRenderPass::finish(optional<string> label)
  {
    return finish(label, 0);
  }

  unique_ptr<GPUCommandBufferBase> TrRenderPass::finish(optional<string> label, int eyeIndex)
  {
    assert(eyeIndex >= 0 && eyeIndex < kMaxEyes && "invalid eye index");
    if (command_encoders_[eyeIndex] == nullptr) [[unlikely]]
    {
      throw std::runtime_error("Failed to finish render pass with an out-of-range eye index: " +
                               std::to_string(eyeIndex));
    }
    else
    {
      return command_encoders_[eyeIndex]->finish(label);
    }
  }

  void TrRenderPass::submit(optional<string> label)
  {
    assert(gpu_device_ != nullptr && xr_device_ != nullptr && "GPU device or XR device is not initialized.");
    auto queue = gpu_device_->queue();
    if (queue == nullptr)
    {
      std::cerr << "Failed to get GPU queue." << std::endl;
      return;
    }

    vector<GPUCommandBufferBase *> raw;

    if (cached_command_buffers_.empty())
    {
      switch (xr_device_->getStereoRenderingMode())
      {
      case xr::TrStereoRenderingMode::MultiPass:
      {
        auto cmd = finish(label, xr_device_->getActiveEyeId());
        if (cmd)
        {
          cached_command_buffers_.push_back(std::move(cmd));
        }
        break;
      }
      case xr::TrStereoRenderingMode::SinglePass:
      case xr::TrStereoRenderingMode::SinglePassInstanced:
      case xr::TrStereoRenderingMode::SinglePassMultiview:
      {
        size_t count = xr_device_->getUsedViewsCount();
        for (size_t i = 0; i < count; ++i)
        {
          auto cmd = finish(label, static_cast<int>(i));
          if (cmd)
          {
            cached_command_buffers_.push_back(std::move(cmd));
          }
        }
        break;
      }
      default:
        break;
      }
    }

    for (const auto &cmd : cached_command_buffers_)
    {
      raw.push_back(cmd.get());
    }

    if (!raw.empty())
      queue->submit(static_cast<uint32_t>(raw.size()), raw.data());
  }
}
