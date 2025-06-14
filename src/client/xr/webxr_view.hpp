#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "./common.hpp"
#include "./webxr_viewport.hpp"
#include "./webxr_rigid_transform.hpp"

namespace client_xr
{
  /**
   * @class XRView
   * The `XRView` class represents a single view into the XR scene for a specific frame.
   * It provides orientation and position information for the viewpoint.
   *
   * @see https://developer.mozilla.org/en-US/docs/Web/API/XRView
   */
  class XRView
  {
  public:
    /**
     * Constructs a new `XRView` object.
     *
     * @param viewData The data of the view.
     * @param session The session that the view belongs to.
     * @param baseReferenceSpace The reference space that the view is based on.
     */
    XRView(xr::TrXRView &viewData,
           std::shared_ptr<XRSession> session,
           std::shared_ptr<XRReferenceSpace> baseReferenceSpace);

  public:
    /**
     * Gets the eye that this view represents.
     *
     * @returns A `XREye` value indicating which eye's viewpoint the `XRView` represents.
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
     * Gets the viewport of this view.
     *
     * @returns An `XRViewport` object describing the size and position of the view.
     */
    inline XRViewport &viewport()
    {
      return viewport_;
    }

    /**
     * Gets the projection matrix of this view.
     *
     * @returns A `glm::mat4` object representing the projection matrix.
     */
    glm::mat4 projectionMatrix() const
    {
      return projectionMatrix_;
    }

    /**
     * Gets the recommended viewport scale for this view.
     *
     * @returns A number greater than 0.0 and less than or equal to 1.0; or `null` if no recommendation is available.
     */
    float recommendedViewportScale() const
    {
      return viewportScale_;
    }

    /**
     * Gets the transform of this view.
     *
     * @returns A `XRRigidTransform` object specifying the position and orientation of the viewpoint.
     */
    XRRigidTransform transform() const
    {
      return transform_;
    }

  public:
    /**
     * Requests a viewport scale for this view.
     *
     * @param scale The scale to request.
     */
    void requestViewportScale(float scale)
    {
      // Default implementation does nothing
    }

  private:
    uint32_t index_;             // Index of the view (0 for left eye, 1 for right eye)
    uint32_t sessionId_;         // ID of the session that the view belongs to
    glm::mat4 projectionMatrix_; // Projection matrix for the view
    XRRigidTransform transform_; // Transform of the view
    XRViewport viewport_;        // Viewport of the view
    float viewportScale_ = 1.0f; // Recommended viewport scale
  };
} // namespace client_xr
