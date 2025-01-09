#pragma once

#include <atomic>
#include <vector>
#include <memory>
#include "common/zone.hpp"
#include "./common.hpp"

using namespace std;

namespace xr
{
  /**
   * The configuration to initialize a XR device.
   */
  class TrDeviceInit final
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
     * The XR session context
     */
    string sessionContextZoneDirectory;
    /**
     * THe XR device context zone path.
     */
    string deviceContextZonePath;
    /**
     * XR input sources zone path.
     */
    string inputSourcesZonePath;

  public:
    /**
     * If the renderer should render contents as multipass mode.
     */
    bool renderedAsMultipass() { return stereoRenderingMode == TrStereoRenderingMode::MultiPass; }
    /**
     * If the renderer should use multiview mode to render contents.
     */
    bool multiviewRequired() { return stereoRenderingMode == TrStereoRenderingMode::SinglePassMultiview || stereoRenderingMode == TrStereoRenderingMode::SinglePassInstanced; }
  };

  class TrXRStereoFrameInfo
  {
  public:
    TrXRStereoFrameInfo()
    {
      memset(viewerBaseMatrix, 0, sizeof(float) * 16);
      views[0].viewIndex = 0;
      views[1].viewIndex = 1;
    }
    ~TrXRStereoFrameInfo() = default;

  public:
    /**
     * Current viewer's base matrix.
     */
    float viewerBaseMatrix[16];
    /**
     * The two views for stereo rendering: [left, right].
     */
    TrXRView views[2];

  public:
    /**
     * Get the view reference by index.
     *
     * @param viewIndex The view index, 0 or 1.
     * @returns The view reference.
     */
    TrXRView &getView(uint32_t viewIndex) { return views[viewIndex]; }
    /**
     * Update the viewer's base matrix.
     *
     * @param matrixValues The matrix values.
     */
    void updateViewerBaseMatrix(float *matrixValues)
    {
      memcpy(viewerBaseMatrix, matrixValues, sizeof(float) * 16);
    }
  };

  class TrXRDeviceContextData
  {
  public:
    TrXRDeviceContextData() = default;
    TrXRDeviceContextData(TrXRDeviceContextData &that)
        : enabled(that.enabled.load()),
          stereoRenderingMode(that.stereoRenderingMode),
          recommendedFov(that.recommendedFov.load()),
          stereoFrame(that.stereoFrame),
          framebufferConf(that.framebufferConf)
    {
    }
    ~TrXRDeviceContextData() = default;

  public:
    /**
     * If this device is enabled.
     */
    atomic<bool> enabled;
    /**
     * The stereo rendering mode.
     */
    TrStereoRenderingMode stereoRenderingMode = TrStereoRenderingMode::MultiPass;
    /**
     * The recommended fov value for client.
     */
    atomic<float> recommendedFov = 0;
    /**
     * The current stereo frame info, the client-side will use this to dispatch `XRFrame`s.
     */
    TrXRStereoFrameInfo stereoFrame;
    /**
     * The framebuffer configuration.
     */
    TrXRFramebufferConfig framebufferConf;
  };

  class TrXRDeviceContextZone : public TrZone<TrXRDeviceContextData>
  {
  public:
    TrXRDeviceContextZone(string filename, TrZoneType type) : TrZone<TrXRDeviceContextData>(filename, type)
    {
      if (type == TrZoneType::Server)
        data = std::make_unique<TrXRDeviceContextData>();
      else
        data.reset(getData());
    }

  public:
    void configure(bool enabled, TrStereoRenderingMode mode)
    {
      data->enabled = enabled;
      data->stereoRenderingMode = mode;
    }
    TrXRView &getStereoView(uint32_t viewIndex) { return data->stereoFrame.getView(viewIndex); }
    void updateRecommendedFov(float fov) { data->recommendedFov = fov; }
    void updateViewerBaseMatrix(float *matrixValues) { data->stereoFrame.updateViewerBaseMatrix(matrixValues); }
    void updateViewFramebuffer(int viewIndex, int framebufferId, TrViewport viewport, bool useDoubleWide)
    {
      assert(viewIndex >= 0 && viewIndex < 2);
      data->framebufferConf.height = viewport.height();
      data->framebufferConf.width = viewport.width();
      data->framebufferConf.useDoubleWide = useDoubleWide;
      auto &view = data->stereoFrame.getView(viewIndex);
      view.viewFramebuffer = framebufferId;
      view.viewport = viewport;
    }
    TrXRStereoFrameInfo &getStereoFrame() { return data->stereoFrame; }
    TrXRFramebufferConfig &getFramebufferConfig() { return data->framebufferConf; }
  };
}
