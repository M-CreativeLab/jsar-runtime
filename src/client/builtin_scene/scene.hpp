#pragma once

#include <memory>
#include <shared_mutex>

#include "./ecs-inl.hpp"
#include "./asset.hpp"
#include "./text.hpp"
#include "./timer.hpp"
#include "./camera.hpp"
#include "./hierarchy.hpp"
#include "./transform.hpp"
#include "./meshes.hpp"
#include "./materials.hpp"
#include "./client_renderer.hpp"
#include "./web_content.hpp"
#include "./xr.hpp"

#include "../graphics/webgl_context.hpp"
#include "../xr/device.hpp"
#include "../xr/webxr_session.hpp"
#include "../per_process.hpp"

namespace builtin_scene
{
  /**
   * The main class for the builtin scene which inherits from the `ecs::App`.
   */
  class Scene : public ecs::App
  {
  public:
    /**
     * Create a new instance of the Scene.
     *
     * @param clientContext The client context to use.
     * @returns The new instance of the Scene.
     */
    static std::shared_ptr<Scene> Make(TrClientContextPerProcess *clientContext)
    {
      if (TR_UNLIKELY(clientContext == nullptr))
        clientContext = TrClientContextPerProcess::Get();
      assert(clientContext != nullptr);
      return std::make_shared<Scene>(clientContext);
    }

  public:
    Scene(TrClientContextPerProcess *clientContext);
    ~Scene() = default;

  public:
    /**
     * Create a new element to the scene for rendering.
     *
     * @param name The tag name of the element.
     * @param parent The parent element of the new element.
     */
    [[nodiscard]] ecs::EntityId createElement(std::string name,
                                              std::optional<ecs::EntityId> parent = std::nullopt);

  private:
    void bootstrap();
    void update(uint32_t time, std::shared_ptr<client_xr::XRFrame> frame);

  private:
    std::shared_ptr<client_graphics::WebGL2Context> glContext_;
    std::shared_ptr<client_xr::XRDeviceClient> xrDeviceClient_;
    std::shared_ptr<client_xr::XRSession> xrSession_;
    client_xr::XRFrameCallback frameCallback_;
  };
}
