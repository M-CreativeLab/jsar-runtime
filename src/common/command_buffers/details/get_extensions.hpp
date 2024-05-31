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

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override {}
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
      if (extensions.size() > 0)
      {
        for (auto exten : extensions)
          message->addStringSegment(exten);
      }
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      auto count = message.getSegmentCount();
      for (int i = 0; i < extensions.size(); i++)
      {
        auto segment = message.getSegment(i);
        if (segment == nullptr)
          extensions[i] = "";
        else
          extensions[i] = segment->toString();
      }
    }

  public:
    vector<string> extensions;
  };
}
