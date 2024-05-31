#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../message.hpp"

namespace commandbuffers
{
  class WebGL2ContextInitCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    WebGL2ContextInitCommandBufferRequest() : TrCommandBufferBase(COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_REQ)
    {
      size = sizeof(WebGL2ContextInitCommandBufferRequest);
    }
    ~WebGL2ContextInitCommandBufferRequest() {}

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }
  };

  class WebGL2ContextInitCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    WebGL2ContextInitCommandBufferResponse(WebGL2ContextInitCommandBufferRequest *req)
        : TrCommandBufferResponse(COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_RES, req)
    {
      size = sizeof(WebGL2ContextInitCommandBufferResponse);
    }
    ~WebGL2ContextInitCommandBufferResponse() {}

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

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
  };
}
