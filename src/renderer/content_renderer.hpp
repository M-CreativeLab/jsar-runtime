#pragma once

#include <vector>
#include "common/classes.hpp"
#include "common/ipc.hpp"
#include "common/command_buffers/shared.hpp"
#include "common/frame_request/types.hpp"
#include "xr/device.hpp"
#include "./gles/context_storage.hpp"

using namespace xr;
using namespace commandbuffers;
using namespace frame_request;

namespace renderer
{
  class TrContentRenderer
  {
  public:
    TrContentRenderer(TrContentRuntime *content, TrConstellation *constellation);

  public: // public lifecycle
    void onCommandBuffersExecuting();
    void onCommandBuffersExecuted();

  public:
    bool sendCommandBufferResponse(TrCommandBufferResponse &res);
    OpenGLAppContextStorage& getOpenGLContext();
    TrContentRuntime* getContent();

  private: // private lifecycle
    void onHostFrame();
    void onStartFrame();
    void onEndFrame();

  private:
    template <typename T>
    void dispatchFrameRequest(TrFrameRequestSimple<T> req)
    {
      if (frameRequestChanSender == nullptr)
        return;
      auto reqMessage = req.serialize();
      frameRequestChanSender->send(*reqMessage);
      delete reqMessage;
    }
    void dispatchAnimationFrameRequest();
    void executeCommandBuffers();

  private:
    void resetFrameRequestChanSender(ipc::TrChannelSender<TrFrameRequestMessage> *sender);

  private:
    TrContentRuntime *content = nullptr;
    TrConstellation *constellation = nullptr;
    xr::Device *xrDevice = nullptr;
    OpenGLAppContextStorage glContext;

  private:
    ipc::TrChannelSender<TrFrameRequestMessage> *frameRequestChanSender = nullptr;

    friend class TrRenderer;
  };
}
