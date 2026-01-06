#pragma once

#include <memory>
#include <optional>
#include <string>
#include <array>

#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_device.hpp>
#include <renderer/render_pass.hpp>
#include <xr/device.hpp>

namespace renderer
{
  class TrRenderFrame final
  {
  public:
    TrRenderFrame(Ref<commandbuffers::GPUDeviceBase> device, xr::Device *xrDevice);
    ~TrRenderFrame();

    Ref<TrRenderPass> opaquePass() const;
    Ref<TrRenderPass> transparentPass() const;
    Ref<TrRenderPass> offscreenPass() const;

    void onOpaquePass(const std::string &label = "opaque");
    void onTransparentPass(const std::string &label = "transparent");
    void onOffscreenPass(const std::string &label = "offscreen");

    void setId(int id) { id_ = id; }
    int getId() const { return id_; }

    void startFrame(int viewIndex);
    void endFrame(int viewIndex);
    bool ended(int viewIndex) const;

  private:
    int id_ = 0;
    std::array<bool, 2> view_ended_ = {false, false};
    Ref<TrRenderPass> opaque_pass_;
    Ref<TrRenderPass> transparent_pass_;
    Ref<TrRenderPass> offscreen_pass_;
  };
}

