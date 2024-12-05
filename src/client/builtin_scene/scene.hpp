#pragma once

#include <memory>
#include "./hierarchy.hpp"
#include "../graphics/webgl_context.hpp"
#include "../xr/device.hpp"
#include "../xr/webxr_session.hpp"
#include "../per_process.hpp"

namespace builtin_scene
{
  class Scene : public Hierarchy
  {
  public:
    /**
     * Create a new instance of the Scene.
     *
     * @param glContext The WebGL2 context to use.
     * @param xrDeviceClient The XR device client to use.
     * @returns The new instance of the Scene.
     */
    static std::shared_ptr<Scene> Make(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                                       std::shared_ptr<client_xr::XRDeviceClient> xrDeviceClient)
    {
      return std::make_shared<Scene>(glContext, xrDeviceClient);
    }

  public:
    Scene(std::shared_ptr<client_graphics::WebGL2Context> glContext,
          std::shared_ptr<client_xr::XRDeviceClient> xrDeviceClient)
        : Hierarchy(),
          glContext_(glContext),
          xrDeviceClient_(xrDeviceClient)
    {
      assert(glContext_ != nullptr);
      assert(xrDeviceClient_ != nullptr);

      auto clientContext = TrClientContextPerProcess::Get();
      assert(clientContext != nullptr);

      auto setupXRSession = [this, clientContext](auto type, auto &event)
      {
        auto xrSystem = xrDeviceClient_->getXRSystem(clientContext->getScriptingEventLoop());
        xrSession_ = xrSystem->requestSession();
        xrSession_->requestAnimationFrame([](uint32_t time, client_xr::XRFrame &frame)
                                          { std::cerr << "frame time: " << time << std::endl; });
      };
      clientContext->addEventListener(TrClientContextEventType::ScriptingEventLoopReady, setupXRSession);
    }
    ~Scene() = default;

  public:
    void update()
    {
    }

  private:
    std::shared_ptr<client_graphics::WebGL2Context> glContext_;
    std::shared_ptr<client_xr::XRDeviceClient> xrDeviceClient_;
    std::shared_ptr<client_xr::XRSession> xrSession_;
  };
}
