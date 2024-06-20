#pragma once

#include "./types.hpp"

namespace frame_request
{
  class TrFrameRequestSender : public ipc::TrChannelSender<TrFrameRequestMessage>
  {
  public:
    TrFrameRequestSender(ipc::TrOneShotClient<TrFrameRequestMessage> *client) : ipc::TrChannelSender<TrFrameRequestMessage>(client) {}
    ~TrFrameRequestSender() {}

  public:
    template <typename T>
    bool sendFrameRequest(TrFrameRequestSimple<T> &frameReq)
    {
      TrFrameRequestMessage *message = frameReq.serialize();
      if (message == nullptr)
        return false;

      void *data = nullptr;
      size_t size = 0;
      bool success = message->serialize(&data, &size);
      delete message; // free message no matter what the result is.
      if (!success)
        return false;

      success = sendRaw(data, size);
      free(data);
      return success;
    }
  };
}
