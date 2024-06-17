#pragma once

#include <vector>
#include "common/classes.hpp"
#include "common/ipc.hpp"
#include "common/command_buffers/shared.hpp"
#include "common/frame_request/types.hpp"
#include "common/frame_request/sender.hpp"
#include "common/xr/types.hpp"
#include "xr/device.hpp"
#include "./gles/context_storage.hpp"

using namespace commandbuffers;
using namespace frame_request;

namespace renderer
{
  class TrContentRenderer
  {
  public:
    TrContentRenderer(TrContentRuntime *content, TrConstellation *constellation);
    ~TrContentRenderer();

  public: // public lifecycle
    void onCommandBuffersExecuting();
    void onCommandBuffersExecuted();

  public:
    bool sendCommandBufferResponse(TrCommandBufferResponse &res);
    OpenGLAppContextStorage *getOpenGLContext();
    TrContentRuntime *getContent();
    pid_t getContentPid();

  private: // private lifecycle
    void onHostFrame();
    void onStartFrame();
    void onEndFrame();

  private:
    template <typename T>
    void dispatchFrameRequest(TrFrameRequestSimple<T> &req)
    {
      if (frameRequestChanSender != nullptr)
        frameRequestChanSender->sendFrameRequest(req);
    }
    void dispatchAnimationFrameRequest();
    void dispatchXRFrameRequest(xr::TrXRSession *session);
    void executeCommandBuffers();

  private:
    void resetFrameRequestChanSenderWith(ipc::TrOneShotClient<TrFrameRequestMessage> *client);

  private:
    TrContentRuntime *content = nullptr;
    TrConstellation *constellation = nullptr;
    OpenGLAppContextStorage *glContext = nullptr;
    xr::Device *xrDevice = nullptr;
    xr::TrXRFrameRequest* currentBaseXRFrameReq = nullptr;

  private:
    frame_request::TrFrameRequestSender *frameRequestChanSender = nullptr;

    friend class TrRenderer;
  };
}
