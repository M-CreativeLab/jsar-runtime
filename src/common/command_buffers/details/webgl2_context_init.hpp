#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class WebGL2ContextInitCommandBufferRequest : public TrCommandBufferSimpleRequest<WebGL2ContextInitCommandBufferRequest>
  {
  public:
    WebGL2ContextInitCommandBufferRequest() : TrCommandBufferSimpleRequest(COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_REQ)
    {
    }
    ~WebGL2ContextInitCommandBufferRequest() {}
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
