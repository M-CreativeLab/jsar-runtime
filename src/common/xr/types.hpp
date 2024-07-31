#pragma once

#include "common/viewport.hpp"
#include "common/frame_request/types.hpp"

#include "./common.hpp"
#include "./input_sources.hpp"

using namespace frame_request;

namespace xr
{
  enum class TrStereoRenderingMode
  {
    MultiPass = 0,
    SinglePass = 1,
    SinglePassInstanced = 2,
    SinglePassMultiview = 3,
    Unknown = -1,
  };

  enum class TrXRSessionMode
  {
    ImmersiveAR = 0,
    ImmersiveVR = 1,
    Inline = 2,
    Unknown = -1,
  };

  inline TrXRSessionMode MakeSessionMode(const string &mode)
  {
    if (mode == "immersive-ar")
      return TrXRSessionMode::ImmersiveAR;
    if (mode == "immersive-vr")
      return TrXRSessionMode::ImmersiveVR;
    if (mode == "inline")
      return TrXRSessionMode::Inline;
    return TrXRSessionMode::Unknown;
  }

  inline bool IsImmersive(TrXRSessionMode mode)
  {
    return mode == TrXRSessionMode::ImmersiveAR || mode == TrXRSessionMode::ImmersiveVR;
  }

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

  /**
   * The configuration to initialize a XR device.
   */
  class TrDeviceInit
  {
  public:
    TrDeviceInit() {}
    ~TrDeviceInit() = default;

  public:
    /**
     * If this device is enabled.
     */
    bool enabled = false;
    /**
     * If this device is active.
     */
    bool active = false;
    /**
     * The XR device's stereo rendering mode.
     */
    TrStereoRenderingMode stereoRenderingMode = TrStereoRenderingMode::Unknown;
    /**
     * Recommended fov value for client.
     */
    float recommendedFov = 0;

  public: // No need to setup, the following fields are set by server.
    /**
     * XR Command channel port.
     */
    int commandChanPort = 0;
    /**
     * XR input sources zone path.
     */
    string inputSourcesZonePath;

  public:
    bool renderedAsMultipass() { return stereoRenderingMode == TrStereoRenderingMode::MultiPass; }
  };

  enum class TrXRViewMatrixType
  {
    View = 0,
    Projection = 1,
  };

  class TrXRView
  {
  public:
    TrXRView(uint32_t viewIndex = 0) : viewIndex(viewIndex)
    {
    }

  public:
    inline void setViewport(TrViewport viewport) { this->viewport = viewport; }
    inline void setViewport(int w, int h, int x = 0, int y = 0) { this->viewport = TrViewport(w, h, x, y); }
    inline void setViewMatrix(float *matrixFloats) { setMatrix(TrXRViewMatrixType::View, matrixFloats); }
    inline void setViewMatrix(glm::mat4 matrix) { setViewMatrix(glm::value_ptr(matrix)); }
    inline void setProjectionMatrix(float *matrixFloats) { setMatrix(TrXRViewMatrixType::Projection, matrixFloats); }
    inline void setProjectionMatrix(glm::mat4 matrix) { setProjectionMatrix(glm::value_ptr(matrix)); }

  public:
    inline glm::mat4 getViewMatrix() { return glm::make_mat4(viewMatrix); }
    inline glm::mat4 getProjectionMatrix() { return glm::make_mat4(projectionMatrix); }

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

  class TrXRFrameRenderingInfo
  {
  public:
    TrXRFrameRenderingInfo() : sessionId(0),
                               stereoId(0),
                               viewIndex(-1)
    {
    }
    TrXRFrameRenderingInfo(TrXRFrameRenderingInfo &that) : sessionId(that.sessionId),
                                                           stereoId(that.stereoId),
                                                           viewIndex(that.viewIndex)
    {
    }
    TrXRFrameRenderingInfo(uint32_t sessionId, int stereoId, int viewIndex) : sessionId(sessionId),
                                                                              stereoId(stereoId),
                                                                              viewIndex(viewIndex)
    {
    }

  public:
    inline bool isValid() { return sessionId > 0 && viewIndex >= 0; }
    inline string toString()
    {
      return "Info(session=" + to_string(sessionId) + "," +
             "stereoId=" + to_string(stereoId) + "," +
             "viewIndex=" + to_string(viewIndex) + ")";
    }

  public:
    uint32_t sessionId;
    uint32_t stereoId;
    uint8_t viewIndex;
  };

  class TrXRFrameRequest : public TrFrameRequestSimple<TrXRFrameRequest>
  {
  public:
    static const size_t ViewsCount = 2;

  public:
    TrXRFrameRequest(TrXRFrameRequest &that)
        : TrFrameRequestSimple(that),
          sessionId(that.sessionId),
          stereoId(that.stereoId),
          viewIndex(that.viewIndex),
          views{that.views[0], that.views[1]},
          framebufferId(that.framebufferId),
          framebufferWidth(that.framebufferWidth),
          framebufferHeight(that.framebufferHeight)
    {
      setLocalBaseMatrix(that.localBaseMatrix);
      setViewerBaseMatrix(that.viewerBaseMatrix);
    }
    TrXRFrameRequest() : TrFrameRequestSimple(TrFrameRequestType::XRFrame) {}

  public: // Methods for server-side only
    /**
     * Clone the current frame request to a new `TrXRFrameRequest` instance.
     */
    TrXRFrameRequest clone()
    {
      TrXRFrameRequest req(*this);
      req.renewId();
      return req;
    }
    void reset()
    {
      sessionId = 0;
      stereoId = 0;
      viewIndex = -1;
      views[0] = TrXRView();
      views[1] = TrXRView();
      setLocalBaseMatrix(glm::mat4(1.0f));
      setViewerBaseMatrix(glm::mat4(1.0f));

      framebufferId = -1;
      framebufferWidth = 0;
      framebufferHeight = 0;
    }
    void setLocalBaseMatrix(float *matrixFloats)
    {
      for (int i = 0; i < 16; i++)
        localBaseMatrix[i] = matrixFloats[i];
    }
    void setLocalBaseMatrix(glm::mat4 matrix) { setLocalBaseMatrix(glm::value_ptr(matrix)); }
    void setViewerBaseMatrix(float *matrixFloats)
    {
      for (int i = 0; i < 16; i++)
        viewerBaseMatrix[i] = matrixFloats[i];
    }
    void setViewerBaseMatrix(glm::mat4 matrix) { setViewerBaseMatrix(glm::value_ptr(matrix)); }

  public: // Methods for client-side only: read matrices
    TrXRFrameRenderingInfo createRenderingInfo(int viewIndex) { return TrXRFrameRenderingInfo(sessionId, stereoId, viewIndex); }

  public:
    uint32_t sessionId;
    int stereoId;
    int viewIndex;
    float localBaseMatrix[16];
    float viewerBaseMatrix[16];
    TrXRView views[ViewsCount];
    int framebufferId = -1;
    int framebufferWidth;
    int framebufferHeight;
  };
}
