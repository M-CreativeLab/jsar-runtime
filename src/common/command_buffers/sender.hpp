#pragma once

#include "./shared.hpp"
#include "./base.hpp"

namespace commandbuffers
{
  class TrCommandBufferSender : public ipc::TrChannelSender<TrCommandBufferMessage>
  {
  public:
    TrCommandBufferSender(ipc::TrOneShotClient<TrCommandBufferMessage> *client)
        : ipc::TrChannelSender<TrCommandBufferMessage>(client)
    {
    }
    ~TrCommandBufferSender()
    {
    }

  public:
    bool sendCommandBufferRequest(TrCommandBufferBase &req, bool forceFlush = false);
    bool sendCommandBufferResponse(TrCommandBufferResponse &res);

  private:
    inline bool needFlush()
    {
      if (pendingBuffersCount > 1000)
        return true;
      auto now = std::chrono::steady_clock::now();
      auto durationInMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFlushTime).count();
      return durationInMs >= 8;
    }
    bool enqueue(void *data, size_t size)
    {
      assert(data != nullptr && size > 0);
      if (bufferToSend == nullptr)
      {
        bufferToSend = data;
        bufferSize = size;
      }
      else
      {
        bufferToSend = realloc(bufferToSend, bufferSize + size);
        if (TR_UNLIKELY(bufferToSend == nullptr))
          return false;
        memcpy((char *)bufferToSend + bufferSize, data, size);
        bufferSize += size;
        free(data);
      }
      pendingBuffersCount += 1;
      return true;
    }
    bool flush()
    {
      pendingBuffersCount = 0;
      lastFlushTime = std::chrono::steady_clock::now();

      if (TR_UNLIKELY(bufferToSend == nullptr))
        return false;

      auto r = sendRaw(bufferToSend, bufferSize);
      free(bufferToSend);
      bufferToSend = nullptr;
      bufferSize = 0;
      return r;
    }

  private:
    void *bufferToSend = nullptr;
    size_t bufferSize = 0;
    int pendingBuffersCount = 0;
    std::chrono::time_point<std::chrono::steady_clock> lastFlushTime;
  };
}
