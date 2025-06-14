#pragma once

#include <atomic>
#include <memory>
#include <shared_mutex>
#include <math/vectors.hpp>
#include <common/collision/ray.hpp>
#include <client/cssom/units.hpp>
#include <client/dom/node.hpp>

#include "./ecs-inl.hpp"
#include "./asset.hpp"
#include "./image.hpp"
#include "./text.hpp"
#include "./timer.hpp"
#include "./camera.hpp"
#include "./hierarchy.hpp"
#include "./transform.hpp"
#include "./bounding_box.hpp"
#include "./meshes.hpp"
#include "./materials.hpp"
#include "./web_content.hpp"
#include "./xr.hpp"

#include "../graphics/webgl_context.hpp"
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
     * Bootstrap the scene, it should be called after you created the scene instance.
     */
    void bootstrap();

    /**
     * Start the scene rendering.
     *
     * @param newSize The new size of the scene.
     */
    void start(std::optional<math::Size3> volumeSize = std::nullopt);

    /**
     * Pause the scene rendering.
     */
    void pause();

    /**
     * Resuming the scene rendering.
     */
    void resume();

    /**
     * Create a new element to the scene for rendering.
     *
     * @param name The tag name of the element.
     * @param node The element's node instance.
     * @param parent The parent element of the new element.
     */
    [[nodiscard]] ecs::EntityId createElement(std::string name,
                                              std::shared_ptr<dom::Node> node,
                                              std::optional<ecs::EntityId> parent = std::nullopt);

    /**
     * Remove the element from the scene.
     *
     * @param entity The entity to remove.
     * @returns Whether the element is removed successfully.
     */
    bool removeElement(ecs::EntityId entity);

    /**
     * @returns The volume size of this scene.
     */
    math::Size3 volumeSize() const
    {
      return volumeSize_;
    }

    /**
     * Update the volume size of the scene.
     *
     * @param size The new volume size.
     */
    void setVolumeSize(std::optional<math::Size3> newSize)
    {
      if (newSize.has_value())
        volumeSize_ = newSize.value();
    }

    /**
     * Get the WebXR experience instance to use.
     *
     * @returns The WebXR experience instance or `nullptr` if not available such as not in XR mode.
     */
    std::shared_ptr<WebXRExperience> getWebXRExperience();

    /**
     * Select a ray for hit testing.
     */
    std::optional<collision::TrRay> selectRayForHitTesting();

    // Events

    typedef std::function<void(client_xr::XRInputSourceEvent &)> SelectEventHandler;
    void onSelectStart(SelectEventHandler);
    void onSelectEnd(SelectEventHandler);

  private:
    void update(uint32_t time, std::shared_ptr<client_xr::XRFrame> frame);
    void setupXRSession();

  private:
    TrClientContextPerProcess *clientContext_ = nullptr;
    std::shared_ptr<client_graphics::WebGL2Context> glContext_;
    std::shared_ptr<client_xr::XRSession> xrSession_;
    client_xr::XRFrameCallback frameCallback_;
    math::Size3 volumeSize_ = math::Size3(client_cssom::ScreenWidth,
                                          client_cssom::ScreenHeight,
                                          client_cssom::VolumeDepth);
    bool started_ = false;
    atomic<bool> paused_ = false;
  };
}
