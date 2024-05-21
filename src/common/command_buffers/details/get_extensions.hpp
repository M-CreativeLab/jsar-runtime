#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../message.hpp"

namespace commandbuffers
{
  class GetExtensionsCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    GetExtensionsCommandBufferRequest() : TrCommandBufferBase(COMMAND_BUFFER_GET_EXTENSIONS_REQ)
    {
      size = sizeof(GetExtensionsCommandBufferRequest);
    }
  };

  class GetExtensionsCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    GetExtensionsCommandBufferResponse(GetExtensionsCommandBufferRequest *req) : TrCommandBufferResponse(COMMAND_BUFFER_GET_EXTENSIONS_RES, req)
    {
      size = sizeof(GetExtensionsCommandBufferResponse);
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      message->addStringSegment(extensions);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      extensions = message.getSegment(0)->toString();
    }

  public:
    string extensions;
  };
}
