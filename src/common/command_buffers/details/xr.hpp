#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class XRFrameStartCommandBufferRequest : public TrCommandBufferSimpleRequest<XRFrameStartCommandBufferRequest>
  {
  public:
    XRFrameStartCommandBufferRequest(int stereoId, int viewIndex)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_XRFRAME_START_REQ),
          stereoId(stereoId),
          viewIndex(viewIndex)
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
  class XRFrameFlushCommandBufferRequest : public TrCommandBufferSimpleRequest<XRFrameFlushCommandBufferRequest>
  {
  public:
    XRFrameFlushCommandBufferRequest(int stereoId, int viewIndex)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_XRFRAME_FLUSH_REQ),
          stereoId(stereoId),
          viewIndex(viewIndex)
    {
    }

  public:
    int stereoId;
    int viewIndex;
  };

  class XRFrameEndCommandBufferRequest : public TrCommandBufferSimpleRequest<XRFrameEndCommandBufferRequest>
  {
  public:
    XRFrameEndCommandBufferRequest(int stereoId, int viewIndex)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_XRFRAME_END_REQ),
          stereoId(stereoId),
          viewIndex(viewIndex)
    {
    }

  public:
    int stereoId;
    int viewIndex;
  };
}
