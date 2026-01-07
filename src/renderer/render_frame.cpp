#include <renderer/render_frame.hpp>

namespace renderer
{
  TrRenderFrame::TrRenderFrame(Ref<commandbuffers::GPUDeviceBase> device, xr::Device *xrDevice)
  {
    opaque_pass_ = AcquireRef(new TrRenderPass(
      RenderPassType::kOpaque,
      "Opaque",
      device,
      xrDevice));
    transparent_pass_ = AcquireRef(new TrRenderPass(
      RenderPassType::kTransparent,
      "Transparent",
      device,
      xrDevice));
    offscreen_pass_ = AcquireRef(new TrRenderPass(
      RenderPassType::kOffscreen,
      "Offscreen",
      device,
      xrDevice));
  }

  TrRenderFrame::~TrRenderFrame()
  {
  }

  Ref<TrRenderPass> TrRenderFrame::opaquePass() const
  {
    return opaque_pass_;
  }

  Ref<TrRenderPass> TrRenderFrame::transparentPass() const
  {
    return transparent_pass_;
  }

  Ref<TrRenderPass> TrRenderFrame::offscreenPass() const
  {
    return offscreen_pass_;
  }

  void TrRenderFrame::onOpaquePass(const std::string &label)
  {
    opaque_pass_->submit(label);
  }

  void TrRenderFrame::onTransparentPass(const std::string &label)
  {
    transparent_pass_->submit(label);
  }

  void TrRenderFrame::onOffscreenPass(const std::string &label)
  {
    offscreen_pass_->submit(label);
  }

  void TrRenderFrame::startFrame(int viewIndex)
  {
    if (viewIndex >= 0 && viewIndex < 2)
      view_ended_[viewIndex] = false;
  }

  void TrRenderFrame::endFrame(int viewIndex)
  {
    if (viewIndex >= 0 && viewIndex < 2)
      view_ended_[viewIndex] = true;
  }

  bool TrRenderFrame::ended(int viewIndex) const
  {
    if (viewIndex >= 0 && viewIndex < 2)
      return view_ended_[viewIndex];
    return true;
  }
}
