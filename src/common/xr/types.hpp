#pragma once

#include "common/viewport.hpp"
#include "common/frame_request/types.hpp"

#include "./common.hpp"
#include "./device_context.hpp"
#include "./session_context.hpp"
#include "./input_sources.hpp"

using namespace frame_request;

namespace xr
{
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
    int8_t viewIndex;
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
