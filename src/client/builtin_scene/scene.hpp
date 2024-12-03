#pragma once

#include <memory>
#include "./hierarchy.hpp"
#include "../graphics/webgl_context.hpp"
#include "../xr/device.hpp"

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
        : Hierarchy(), glContext(glContext), xrDeviceClient(xrDeviceClient)
    {
      assert(glContext != nullptr);
      assert(xrDeviceClient != nullptr);
    }
    ~Scene() = default;

  public:
    void update()
    {
    }

  private:
    std::shared_ptr<client_graphics::WebGL2Context> glContext;
    std::shared_ptr<client_xr::XRDeviceClient> xrDeviceClient;
  };
}
