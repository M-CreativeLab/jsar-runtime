#pragma once

#include <memory>
#include <vector>
#include <xr/render_state.hpp>
#include "./common.hpp"
#include "./webxr_layers.hpp"

namespace client_xr
{
  /**
   * The `XRRenderState` interface of the WebXR Device API contains configurable values which affect how the imagery generated by
   * an `XRSession` gets composited. These properties include the range of distances from the viewer within which content should
   * be rendered, the vertical field of view (for inline presentations), and a reference to the `XRWebGLLayer` being used as the
   * target for rendering the scene prior to it being presented on the XR device's display or displays.
   *
   * When you apply changes using the `XRSession` method `updateRenderState()`, the specified changes take effect after the
   * current animation frame has completed, but before the next one begins.
   */
  class XRRenderState
  {
  public:
    XRRenderState()
        : baseLayer(nullptr)
        , depthFar(1000.0)
        , depthNear(0.1)
        , inlineVerticalFieldOfView(0)
    {
    }
    XRRenderState(xr::RenderState *stateData)
        : baseLayer(std::make_shared<XRWebGLLayer>(*stateData->baseLayer))
        , depthFar(stateData->depthFar)
        , depthNear(stateData->depthNear)
        , inlineVerticalFieldOfView(stateData->inlineVerticalFieldOfView)
    {
    }
    XRRenderState(XRRenderState &state)
        : baseLayer(state.baseLayer)
        , depthFar(state.depthFar)
        , depthNear(state.depthNear)
        , inlineVerticalFieldOfView(state.inlineVerticalFieldOfView)
    {
    }

  public:
    void update(XRRenderState &newState)
    {
      if (newState.baseLayer != nullptr && baseLayer != newState.baseLayer)
        baseLayer = newState.baseLayer;
      if (depthFar != newState.depthFar)
        depthFar = newState.depthFar;
      if (depthNear != newState.depthNear)
        depthNear = newState.depthNear;
      if (inlineVerticalFieldOfView != newState.inlineVerticalFieldOfView)
        inlineVerticalFieldOfView = newState.inlineVerticalFieldOfView;
    }

  public:
    /**
     * The `XRWebGLLayer` from which the browser's compositing system obtains the image for the XR session.
     */
    std::shared_ptr<XRWebGLLayer> baseLayer;
    /**
     * The distance, in meters, of the __far clip plane__ from the viewer. The far clip plane is the plane which is parallel to
     * the display beyond which rendering of the scene no longer takes place. This, essentially, specifies the maximum distance
     * the user can see.
     */
    float depthFar;
    /**
     * The distance, in meters, of the near clip plane from the viewer. The near clip plane is the plane, parallel to the display,
     * at which rendering of the scene begins. Any closer to the viewer than this, and no portions of the scene are drawn.
     */
    float depthNear;
    /**
     * The default vertical field of view, defined in radians, to use when the session is in `inline` mode. `null` for all immersive
     * sessions.
     */
    float inlineVerticalFieldOfView;
#ifdef TR_WEBXR_LAYERS
    /**
     * An ordered array containing `XRLayer` objects that are displayed by the XR compositor.
     */
    std::vector<std::shared_ptr<XRLayer>> layers;
#endif
  };
}
