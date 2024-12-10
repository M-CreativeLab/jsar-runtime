#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "./common.hpp"
#include "./webxr_viewport.hpp"
#include "./webxr_rigid_transform.hpp"

namespace client_xr
{
  /**
   * The WebXR Device API's `XRView` interface describes a single view into the XR scene for a specific frame, providing
   * orientation and position information for the viewpoint. You can think of it as a description of a specific eye or camera
   * and how it views the world. A 3D frame will involve two views, one for each eye, separated by an appropriate distance which
   * approximates the distance between the viewer's eyes. This allows the two views, when projected in isolation into the
   * appropriate eyes, to simulate a 3D world.
   *
   * @see https://developer.mozilla.org/en-US/docs/Web/API/XRView
   */
  class XRView
  {
  public:
    /**
     * Create a new `XRView` object.
     *
     * @param viewData the data of the view
     * @param session the session that the view belongs to
     * @param baseReferenceSpace the reference space that the view is based on
     */
    XRView(xr::TrXRView &viewData,
           std::shared_ptr<XRSession> session,
           std::shared_ptr<XRReferenceSpace> baseReferenceSpace);

  public:
    /**
     * The `XRView` interface's read-only `eye` property is a `XREye` value indicating which eye's viewpoint the `XRView`
     * represents: left or right. For views which represent neither eye, such as monoscopic views, this property's value
     * is none.
     *
     * @returns A `XREye` value.
     */
    inline XREye eye() const
    {
      if (index_ == 0)
        return XREye::kLeft;
      else if (index_ == 1)
        return XREye::kRight;
      else
        return XREye::kNone;
    }
    /**
     * The `XRView` interface's read-only `viewport` property is an `XRViewport` object which describes the size and position
     * of the view within the overall XR display. The viewport is specified in pixels, with the origin at the top-left corner
     * of the display.
     * 
     * @returns An `XRViewport` object.
     */
    inline XRViewport viewport() const { return viewport_; }
    /**
     * The `XRView` interface's read-only `projectionMatrix` property specifies the projection matrix to apply to the underlying
     * view. This should be used to integrate perspective to everything in the scene, in order to ensure the result is consistent
     * with what the eye expects to see.
     *
     * @returns an `glm::mat4` object.
     */
    glm::mat4 projectionMatrix() const { return projectionMatrix_; }
    /**
     * The read-only `recommendedViewportScale` property of the `XRView` interface is the recommended viewport scale value that
     * you can use for `XRView.requestViewportScale()` if the user agent has such a recommendation; `null` otherwise.
     *
     * @returns A number greater than 0.0 and less than or equal to 1.0; or `null` if the user agent does not provide a recommended
     *          scale.
     */
    float recommendedViewportScale() const { return viewportScale_; }
    /**
     * The read-only `transform` property of the `XRView` interface is an `XRRigidTransform` object which provides the position
     * and orientation of the viewpoint relative to the `XRReferenceSpace` specified when the `XRFrame.getViewerPose()` method
     * was called to obtain the view object.
     *
     * With the `transform`, you can then position the view as a camera within the 3D scene. If you instead need the more
     * traditional view matrix, you can get using `view.transform.inverse.matrix`; this gets the underlying matrix of the
     * transform's inverse.
     *
     * @returns A `XRRigidTransform` object specifying the position and orientation of the viewpoint represented by the `XRView`.
     */
    XRRigidTransform transform() const { return transform_; }

  public:
    void requestViewportScale(float scale)
    {
      throw std::runtime_error("Not implemented");
    }

  private:
    uint32_t index_;
    uint32_t sessionId_;
    glm::mat4 projectionMatrix_;
    XRRigidTransform transform_;
    XRViewport viewport_;
    float viewportScale_ = 1.0f;
  };
}
