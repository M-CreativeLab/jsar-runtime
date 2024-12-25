#pragma once

#include "common/viewport.hpp"
#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class CreateWebGLContextRequest : public TrCommandBufferSimpleRequest<CreateWebGLContextRequest,
                                                                        COMMAND_BUFFER_CREATE_WEBGL_CONTEXT_REQ>
  {
  public:
    using TrCommandBufferSimpleRequest::TrCommandBufferSimpleRequest;

    CreateWebGLContextRequest(uint32_t contextId)
        : TrCommandBufferSimpleRequest(),
          contextId(contextId)
    {
    }

  public:
    uint32_t contextId;
  };

  class WebGL1ContextInitCommandBufferRequest : public TrCommandBufferSimpleRequest<WebGL1ContextInitCommandBufferRequest,
                                                                                    COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ>
  {
  public:
    using TrCommandBufferSimpleRequest::TrCommandBufferSimpleRequest;
  };

  class WebGL1ContextInitCommandBufferResponse : public TrCommandBufferSimpleResponse<WebGL1ContextInitCommandBufferResponse>
  {
  public:
    WebGL1ContextInitCommandBufferResponse(WebGL1ContextInitCommandBufferRequest *req)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES, req)
    {
    }
    WebGL1ContextInitCommandBufferResponse(WebGL1ContextInitCommandBufferResponse &that)
        : TrCommandBufferSimpleResponse(that),
          drawingViewport(that.drawingViewport),
          maxCombinedTextureImageUnits(that.maxCombinedTextureImageUnits),
          maxCubeMapTextureSize(that.maxCubeMapTextureSize),
          maxFragmentUniformVectors(that.maxFragmentUniformVectors),
          maxRenderbufferSize(that.maxRenderbufferSize),
          maxTextureImageUnits(that.maxTextureImageUnits),
          maxTextureSize(that.maxTextureSize),
          maxVaryingVectors(that.maxVaryingVectors),
          maxVertexAttribs(that.maxVertexAttribs),
          maxVertexTextureImageUnits(that.maxVertexTextureImageUnits),
          maxVertexUniformVectors(that.maxVertexUniformVectors)
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
    TrViewport drawingViewport;
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

  class WebGL2ContextInitCommandBufferRequest : public TrCommandBufferSimpleRequest<WebGL2ContextInitCommandBufferRequest,
                                                                                    COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_REQ>
  {
  public:
    using TrCommandBufferSimpleRequest::TrCommandBufferSimpleRequest;
  };

  class WebGL2ContextInitCommandBufferResponse : public TrCommandBufferSimpleResponse<WebGL2ContextInitCommandBufferResponse>
  {
  public:
    WebGL2ContextInitCommandBufferResponse(WebGL2ContextInitCommandBufferRequest *req)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_RES, req)
    {
    }
    ~WebGL2ContextInitCommandBufferResponse() {}

  public:
    int max3DTextureSize;
    int maxArrayTextureLayers;
    int maxColorAttachments;
    int maxCombinedUniformBlocks;
    int maxDrawBuffers;
    int maxElementsIndices;
    int maxElementsVertices;
    int maxFragmentInputComponents;
    int maxFragmentUniformBlocks;
    int maxFragmentUniformComponents;
    int maxProgramTexelOffset;
    int maxSamples;
    int maxTransformFeedbackInterleavedComponents;
    int maxTransformFeedbackSeparateAttributes;
    int maxTransformFeedbackSeparateComponents;
    int maxUniformBufferBindings;
    int maxVaryingComponents;
    int maxVertexOutputComponents;
    int maxVertexUniformBlocks;
    int maxVertexUniformComponents;
    int minProgramTexelOffset;
    int64_t maxClientWaitTimeout;
    int64_t maxCombinedFragmentUniformComponents;
    int64_t maxCombinedVertexUniformComponents;
    int64_t maxElementIndex;
    int64_t maxServerWaitTimeout;
    int64_t maxUniformBlockSize;
    float maxTextureLODBias;

  public: // Extensions
    int32_t OVR_maxViews;
  };
}
