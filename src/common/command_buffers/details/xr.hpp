#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class XRFrameStartCommandBufferRequest : public TrCommandBufferSimpleRequest<XRFrameStartCommandBufferRequest,
                                                                               COMMAND_BUFFER_XRFRAME_START_REQ>
  {
  public:
    XRFrameStartCommandBufferRequest() = delete;
    XRFrameStartCommandBufferRequest(int stereoId, int viewIndex)
        : TrCommandBufferSimpleRequest()
        , stereoId(stereoId)
        , viewIndex(viewIndex)
    {
    }
    XRFrameStartCommandBufferRequest(const XRFrameStartCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , stereoId(that.stereoId)
        , viewIndex(that.viewIndex)
    {
    }

  public:
    int stereoId;
    int viewIndex;
  };

  /**
   * The flush command buffer in XRFrame means immediately flush(execute) the command buffers in the server-side list, this is
   * useful to submit a command buffer that expects a server response in XRFrame.
   */
  class XRFrameFlushCommandBufferRequest : public TrCommandBufferSimpleRequest<XRFrameFlushCommandBufferRequest,
                                                                               COMMAND_BUFFER_XRFRAME_FLUSH_REQ>
  {
  public:
    XRFrameFlushCommandBufferRequest() = delete;
    XRFrameFlushCommandBufferRequest(int stereoId, int viewIndex)
        : TrCommandBufferSimpleRequest()
        , stereoId(stereoId)
        , viewIndex(viewIndex)
    {
    }
    XRFrameFlushCommandBufferRequest(const XRFrameFlushCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , stereoId(that.stereoId)
        , viewIndex(that.viewIndex)
    {
    }

  public:
    int stereoId;
    int viewIndex;
  };

  class XRFrameEndCommandBufferRequest : public TrCommandBufferSimpleRequest<XRFrameEndCommandBufferRequest,
                                                                             COMMAND_BUFFER_XRFRAME_END_REQ>
  {
  public:
    XRFrameEndCommandBufferRequest() = delete;
    XRFrameEndCommandBufferRequest(int stereoId, int viewIndex)
        : TrCommandBufferSimpleRequest()
        , stereoId(stereoId)
        , viewIndex(viewIndex)
    {
    }
    XRFrameEndCommandBufferRequest(const XRFrameEndCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , stereoId(that.stereoId)
        , viewIndex(that.viewIndex)
    {
    }

  public:
    int stereoId;
    int viewIndex;
  };
}
