#pragma once

#include <chrono>
#include "./ecs.hpp"
#include "../xr/webxr_session.hpp"
#include "../xr/webxr_frame.hpp"

namespace builtin_scene
{
  class WebXRExperience : public ecs::Resource
  {
    friend class Scene;

  public:
    WebXRExperience() : ecs::Resource(),
                        referenceSpace_(nullptr),
                        currentFrame_(nullptr),
                        currentTime_(0)
    {
    }

  public:
    inline std::shared_ptr<client_xr::XRReferenceSpace> referenceSpace() { return referenceSpace_; }
    inline std::shared_ptr<client_xr::XRFrame> currentFrame() { return currentFrame_; }
    inline uint32_t currentTime() { return currentTime_; }
    std::shared_ptr<client_xr::XRViewerPose> viewerPose()
    {
      if (currentFrame_ == nullptr || referenceSpace_ == nullptr)
        return nullptr;
      return currentFrame_->getViewerPose(referenceSpace_);
    }

  private:
    void updateReferenceSpace(std::shared_ptr<client_xr::XRReferenceSpace> space)
    {
      referenceSpace_ = space;
    }
    void updateCurrentFrame(uint32_t time, std::shared_ptr<client_xr::XRFrame> frame)
    {
      currentTime_ = time;
      currentFrame_ = frame;
    }

  public:
    std::shared_ptr<client_xr::XRReferenceSpace> referenceSpace_;
    std::shared_ptr<client_xr::XRFrame> currentFrame_;
    uint32_t currentTime_;
  };

  class WebXRExperienceStartupSystem : public ecs::System
  {
  public:
    using ecs::System::System;

  public:
    void onExecute()
    {
    }
  };

  class WebXRExperienceUpdateSystem : public ecs::System
  {
  public:
    using ecs::System::System;

  public:
    void onExecute()
    {
      auto xrExperience = getResource<WebXRExperience>();
      assert(xrExperience != nullptr);

      auto xrFrame = xrExperience->currentFrame();
      auto referenceSpace = xrExperience->referenceSpace();
      if (xrFrame == nullptr || referenceSpace == nullptr)
        return;

      auto viewerPose = xrFrame->getViewerPose(referenceSpace);
      if (viewerPose == nullptr)
        return;
    }
  };

  class WebXRPlugin : public ecs::Plugin
  {
  public:
    using ecs::Plugin::Plugin;

  protected:
    void build(ecs::App &app) override
    {
      using namespace ecs;

      // Resources
      app.addResource(Resource::Make<WebXRExperience>());

      // Systems
      app.addSystem(SchedulerLabel::kStartup, System::Make<WebXRExperienceStartupSystem>());
      app.addSystem(SchedulerLabel::kPreUpdate, System::Make<WebXRExperienceUpdateSystem>());
    }
  };
}
