#pragma once

#include "common/viewport.hpp"
#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class WebGL1ContextInitCommandBufferRequest : public TrCommandBufferSimpleRequest<WebGL1ContextInitCommandBufferRequest>
  {
  public:
    WebGL1ContextInitCommandBufferRequest() : TrCommandBufferSimpleRequest(COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ) {}
    ~WebGL1ContextInitCommandBufferRequest() {}
  };

  class WebGL1ContextInitCommandBufferResponse : public TrCommandBufferSimpleResponse<WebGL1ContextInitCommandBufferResponse>
  {
  public:
    WebGL1ContextInitCommandBufferResponse(WebGL1ContextInitCommandBufferRequest *req)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES, req)
    {
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
    TrViewport viewport;
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
