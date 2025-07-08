#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class GetExtensionsCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<GetExtensionsCommandBufferRequest,
                                            COMMAND_BUFFER_GET_EXTENSIONS_REQ>
  {
    using TrCommandBufferSimpleRequest::TrCommandBufferSimpleRequest;
  };

  class GetExtensionsCommandBufferResponse final
      : public TrCommandBufferSimpleResponse<GetExtensionsCommandBufferResponse>
  {
  public:
    GetExtensionsCommandBufferResponse(GetExtensionsCommandBufferRequest *req)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_EXTENSIONS_RES, req)
    {
    }
    GetExtensionsCommandBufferResponse(const GetExtensionsCommandBufferResponse &that, bool clone = false)
        : TrCommandBufferSimpleResponse(that, clone)
    {
      if (clone)
        extensions = that.extensions;
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
    std::vector<std::string> extensions;
  };
}
