#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../message.hpp"

namespace commandbuffers
{
  class WebGL1ContextInitCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    WebGL1ContextInitCommandBufferRequest() : TrCommandBufferBase(COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ)
    {
      size = sizeof(WebGL1ContextInitCommandBufferRequest);
    }
    ~WebGL1ContextInitCommandBufferRequest() {}

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }
  };

  class WebGL1ContextInitCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    WebGL1ContextInitCommandBufferResponse(WebGL1ContextInitCommandBufferRequest *req)
        : TrCommandBufferResponse(COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES, req)
    {
      size = sizeof(WebGL1ContextInitCommandBufferResponse);
    }
    ~WebGL1ContextInitCommandBufferResponse()
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      message->addStringSegment(vendor);
      message->addStringSegment(version);
      message->addStringSegment(renderer);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      vendor = message.getSegment(0)->toString();
      version = message.getSegment(1)->toString();
      renderer = message.getSegment(2)->toString();
    }

  public:
    int maxCombinedTextureImageUnits;
    int maxCubeMapTextureSize;
    int maxFragmentUniformVectors;
    int maxRenderbufferSize;
    int maxTextureImageUnits;
    int maxTextureSize;
    int maxVaryingVectors;
    int maxVertexAttribs;
    int maxVertexTextureImageUnits;
    int maxVertexUniformVectors;
    string vendor;
    string version;
    string renderer;
  };
}
