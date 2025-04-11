#pragma once

#include <chrono>
#include <memory>
#include <optional>

#include <common/collision/ray.hpp>
#include <client/per_process.hpp>
#include <client/xr/device.hpp>
#include <client/xr/webxr_session.hpp>
#include <client/xr/webxr_frame.hpp>
#include <client/xr/webxr_spaces.hpp>
#include <client/xr/webxr_input_sources.hpp>

#include "./ecs.hpp"

namespace builtin_scene
{
  class WebXRExperience : public ecs::Resource
  {
    friend class Scene;

  public:
    WebXRExperience();

  public:
    std::shared_ptr<client_xr::XRSession> session() { return session_; }
    client_xr::XRSession& sessionRef() { return *session_; }

    std::shared_ptr<client_xr::XRReferenceSpace> referenceSpace() { return reference_space_; }
    const client_xr::XRReferenceSpace& referenceSpaceRef() { return *reference_space_; }

    std::shared_ptr<client_xr::XRFrame> currentFrame() { return current_frame_; }
    const client_xr::XRFrame& currentFrameRef() { return *current_frame_; }

    uint32_t currentTime() { return current_time_; }
    std::shared_ptr<client_xr::XRViewerPose> viewerPose()
    {
      if (current_frame_ == nullptr || reference_space_ == nullptr)
        return nullptr;
      return current_frame_->getViewerPose(reference_space_);
    }

    bool multiviewEnabled() const { return multiview_enabled_; }
    bool multiviewRequired() const;

    // Request a session
    std::shared_ptr<client_xr::XRSession> requestSession();

    // Select a ray for hit testing
    std::optional<collision::TrRay> selectRayForHitTesting();

  private:
    inline void updateReferenceSpace(std::shared_ptr<client_xr::XRReferenceSpace> space)
    {
      reference_space_ = space;
    }
    inline void updateCurrentFrame(uint32_t time, std::shared_ptr<client_xr::XRFrame> frame)
    {
      current_time_ = time;
      current_frame_ = frame;
    }
    inline void enableMultiview(bool enabled)
    {
      multiview_enabled_ = enabled;
    }

  private:
    std::shared_ptr<client_xr::XRDeviceClient> client_;
    std::shared_ptr<client_xr::XRSession> session_;
    std::shared_ptr<client_xr::XRReferenceSpace> reference_space_;
    std::shared_ptr<client_xr::XRFrame> current_frame_;
    uint32_t current_time_;
    bool multiview_enabled_;
  };

  class WebXRExperienceStartupSystem : public ecs::System
  {
  public:
    using ecs::System::System;

  public:
    const std::string name() const override { return "WebXRExperienceStartupSystem"; }
    void onExecute() override;
  };

  class WebXRExperienceUpdateSystem : public ecs::System
  {
  public:
    using ecs::System::System;

  public:
    const std::string name() const override { return "WebXRExperienceUpdateSystem"; }
    void onExecute() override;
  };

  class WebXRPlugin : public ecs::Plugin
  {
  public:
    using ecs::Plugin::Plugin;

  protected:
    void build(ecs::App &) override;
  };
}
