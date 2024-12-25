#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class GetExtensionsCommandBufferRequest : public TrCommandBufferSimpleRequest<GetExtensionsCommandBufferRequest,
                                                                                COMMAND_BUFFER_GET_EXTENSIONS_REQ>
  {
  public:
    using TrCommandBufferSimpleRequest::TrCommandBufferSimpleRequest;
  };

  class GetExtensionsCommandBufferResponse : public TrCommandBufferSimpleResponse<GetExtensionsCommandBufferResponse>
  {
  public:
    GetExtensionsCommandBufferResponse(GetExtensionsCommandBufferResponse &that) : TrCommandBufferSimpleResponse(that) {}
    GetExtensionsCommandBufferResponse(GetExtensionsCommandBufferRequest *req)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_EXTENSIONS_RES, req)
    {
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
      for (int i = 0; i < count; i++)
      {
        auto segment = message.getSegment(i);
        if (segment != nullptr)
          extensions.push_back(segment->toString());
      }
    }

  public:
    vector<string> extensions;
  };
}
