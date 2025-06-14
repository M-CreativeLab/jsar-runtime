#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace std;

namespace xr
{
  enum class TrHandness
  {
    Left = 0,
    Right = 1,
    None = 2,
  };

  enum class TrStereoRenderingMode
  {
    MultiPass = 0,
    SinglePass = 1,
    SinglePassInstanced = 2,
    SinglePassMultiview = 3,
    Unknown = -1,
  };

  enum class TrXRViewMatrixType
  {
    View = 0,
    Projection = 1,
  };

  enum class TrXRSessionMode
  {
    ImmersiveAR = 0,
    ImmersiveVR = 1,
    Inline = 2,
    Unknown = -1,
  };

  /**
   * The WebXR features that the session could request.
   */
  enum class TrXRFeature
  {
    /**
     * Enable use of XRAnchor objects.
     */
    ANCHORS = 0,
    /**
     * Similar to the local type, except the user is not expected to move outside a predetermined boundary, given by the boundsGeometry in the returned object.
     */
    BOUNDED_FLOOR,
    /**
     * Enable the ability to obtain depth information using XRDepthInformation objects.
     */
    DEPTH_SENSING,
    /**
     * Enable allowing to specify a DOM overlay element that will be displayed to the user.
     */
    DOM_OVERLAY,
    /**
     * Enable articulated hand pose information from hand-based input controllers
     */
    HAND_TRACKING,
    /**
     * Enable hit testing features for performing hit tests against real-world geometry.
     */
    HIT_TEST,
    /**
     * Enable the ability to create various layer types (other than XRProjectionLayer).
     */
    LAYERS,
    /**
     * Enable the ability to estimate environmental lighting conditions using XRLightEstimate objects.
     */
    LIGHT_ESTIMATION,
    /**
     * Enable a tracking space whose native origin is located near the viewer's position at the time the session was created. The exact position depends on the underlying
     * platform and implementation. The user isn't expected to move much if at all beyond their starting position, and tracking is optimized for this use case.
     */
    LOCAL,
    /**
     * Similar to the `LOCAL` type, except the starting position is placed in a safe location for the viewer to stand, where the value of the y axis is 0 at floor level. If
     * that floor level isn't known, the user agent will estimate the floor level. If the estimated floor level is non-zero, the browser is expected to round it such a way
     * as to avoid fingerprinting (likely to the nearest centimeter).
     */
    LOCAL_FLOOR,
    /**
     * Enable XRView objects to be secondary views. This can be used for first-person observer views used for video capture, or "quad views" where there are two views per eye,
     * with differing resolution and fields of view.
     */
    SECONDARY_VIEWS,
    /**
     * Enable a tracking space which allows the user total freedom of movement, possibly over extremely long distances from their origin point. The viewer isn't tracked at all;
     * tracking is optimized for stability around the user's current position, so the native origin may drift as needed to accommodate that need.
     */
    UNBOUNDED,
    /**
     * Enable a tracking space whose native origin tracks the viewer's position and orientation.
     */
    VIEWER,
  };

  /**
   * Convert the `TrXRFeature` enum to a string.
   * 
   * @param feature The feature to convert.
   * @returns The string representation of the feature.
   */
  inline std::string to_string(TrXRFeature feature)
  {
    switch (feature)
    {
    case TrXRFeature::ANCHORS:
      return "anchors";
    case TrXRFeature::BOUNDED_FLOOR:
      return "bounded-floor";
    case TrXRFeature::DEPTH_SENSING:
      return "depth-sensing";
    case TrXRFeature::DOM_OVERLAY:
      return "dom-overlay";
    case TrXRFeature::HAND_TRACKING:
      return "hand-tracking";
    case TrXRFeature::HIT_TEST:
      return "hit-test";
    case TrXRFeature::LAYERS:
      return "layers";
    case TrXRFeature::LIGHT_ESTIMATION:
      return "light-estimation";
    case TrXRFeature::LOCAL:
      return "local";
    case TrXRFeature::LOCAL_FLOOR:
      return "local-floor";
    case TrXRFeature::SECONDARY_VIEWS:
      return "secondary-views";
    case TrXRFeature::UNBOUNDED:
      return "unbounded";
    case TrXRFeature::VIEWER:
      return "viewer";
    default:
      return "unknown";
    }
  }

  class TrXRSessionInit
  {
  public:
    TrXRSessionInit()
    {
      requiredFeatures.push_back(TrXRFeature::LOCAL);
      requiredFeatures.push_back(TrXRFeature::VIEWER);
    }

  public:
    /**
     * An array of `TrXRFeature` values which the returned `XRSession` must support.
     */
    vector<TrXRFeature> requiredFeatures;
    /**
     * An array of `TrXRFeature` values identifying features which the returned `XRSession` may optionally support.
     */
    vector<TrXRFeature> optionalFeatures;
  };

  class TrXRFramebufferConfig
  {
  public:
    TrXRFramebufferConfig() = default;

  public:
    void update(int id, int width, int height)
    {
      this->id = id;
      this->width = width;
      this->height = height;
    }

  public:
    int id = -1;
    int width;
    int height;
    bool useDoubleWide = false;
  };

  class TrXRView
  {
  public:
    TrXRView(uint32_t viewIndex = 0)
        : viewIndex(viewIndex)
    {
    }

  public:
    inline void setViewport(TrViewport viewport)
    {
      this->viewport = viewport;
    }
    inline void setViewport(int w, int h, int x = 0, int y = 0)
    {
      this->viewport = TrViewport(w, h, x, y);
    }
    inline void setViewMatrix(float *matrixFloats)
    {
      setMatrix(TrXRViewMatrixType::View, matrixFloats);
    }
    inline void setViewMatrix(glm::mat4 matrix)
    {
      setViewMatrix(glm::value_ptr(matrix));
    }
    inline void setProjectionMatrix(float *matrixFloats)
    {
      setMatrix(TrXRViewMatrixType::Projection, matrixFloats);
    }
    inline void setProjectionMatrix(glm::mat4 matrix)
    {
      setProjectionMatrix(glm::value_ptr(matrix));
    }

  public:
    inline glm::mat4 getViewMatrix()
    {
      return glm::make_mat4(viewMatrix);
    }
    inline glm::mat4 getProjectionMatrix()
    {
      return glm::make_mat4(projectionMatrix);
    }

  private:
    inline void setMatrix(TrXRViewMatrixType type, float *matrixFloats)
    {
      float *matrixToSet = type == TrXRViewMatrixType::View ? viewMatrix : projectionMatrix;
      for (int i = 0; i < 16; i++)
        matrixToSet[i] = matrixFloats[i];
    }

  public:
    TrViewport viewport;
    uint32_t viewIndex;
    uint32_t viewFramebuffer;
    float projectionMatrix[16];
    float viewMatrix[16];
  };
}
