#pragma once

#include <memory>
#include "./ecs-inl.hpp"
#include "./hierarchy.hpp"
#include "./transform.hpp"

#include "../graphics/webgl_context.hpp"
#include "../xr/device.hpp"
#include "../xr/webxr_session.hpp"
#include "../per_process.hpp"

namespace builtin_scene
{
  /**
   * The `DefaultPlugin` loads the default components and systems for the builtin scene.
   */
  class DefaultPlugin : public ecs::Plugin
  {
  public:
    using ecs::Plugin::Plugin;

  protected:
    void build(ecs::App &app) override
    {
      app.registerComponent<transform::Position>();
    }
  };

  /**
   * The main class for the builtin scene which inherits from the `ecs::App`.
   */
  class Scene : public ecs::App,
                public Hierarchy
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
        : ecs::App(),
          Hierarchy(),
          glContext_(glContext),
          xrDeviceClient_(xrDeviceClient)
    {
      assert(glContext_ != nullptr);
      assert(xrDeviceClient_ != nullptr);

      auto clientContext = TrClientContextPerProcess::Get();
      assert(clientContext != nullptr);

      frameCallback_ = [this](uint32_t time, std::shared_ptr<client_xr::XRFrame> frame, void *env_)
      {
        assert(xrSession_ != nullptr); // ensure the WebXR session is ready.
        update(time, *frame);
        xrSession_->requestAnimationFrame(frameCallback_);
      };

      auto setupXRSession = [this, clientContext](auto type, auto event)
      {
        auto xrSystem = xrDeviceClient_->getXRSystem(clientContext->getScriptingEventLoop());
        xrSession_ = xrSystem->requestSession();

        // Update the render state
        client_xr::XRRenderState renderState;
        renderState.baseLayer = client_xr::XRWebGLLayer::Make(xrSession_, glContext_);
        xrSession_->updateRenderState(renderState);

        bootstrap();
        xrSession_->requestAnimationFrame(frameCallback_);
      };
      clientContext->addEventListener(TrClientContextEventType::ScriptingEventLoopReady, setupXRSession);
    }
    ~Scene() = default;

  public:
    void bootstrap()
    {
      addPlugin<DefaultPlugin>();
      // addSystem(ecs::SchedulerLabel::kUpdate, std::make_shared<LayoutSystem>());
      ecs::App::startup();
    }
    void update(uint32_t time, client_xr::XRFrame &frame)
    {
      ecs::App::update();
    }

  private:
    std::shared_ptr<client_graphics::WebGL2Context> glContext_;
    std::shared_ptr<client_xr::XRDeviceClient> xrDeviceClient_;
    std::shared_ptr<client_xr::XRSession> xrSession_;
    client_xr::XRFrameCallback frameCallback_;
  };
}
