#pragma once

#include "common/viewport.hpp"
#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class CreateWebGLContextRequest final
      : public TrCommandBufferSimpleRequest<CreateWebGLContextRequest, COMMAND_BUFFER_CREATE_WEBGL_CONTEXT_REQ>
  {
    using TrCommandBufferSimpleRequest::TrCommandBufferSimpleRequest;
  };

  class RemoveWebGLContextRequest final
      : public TrCommandBufferSimpleRequest<RemoveWebGLContextRequest, COMMAND_BUFFER_REMOVE_WEBGL_CONTEXT_REQ>
  {
    using TrCommandBufferSimpleRequest::TrCommandBufferSimpleRequest;
  };

  class WebGL1ContextInitCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<WebGL1ContextInitCommandBufferRequest,
                                            COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ>
  {
    using TrCommandBufferSimpleRequest::TrCommandBufferSimpleRequest;
  };

  class WebGL1ContextInitCommandBufferResponse final
      : public TrCommandBufferSimpleResponse<WebGL1ContextInitCommandBufferResponse>
  {
  public:
    WebGL1ContextInitCommandBufferResponse(WebGL1ContextInitCommandBufferRequest *req)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES, req)
    {
    }
    WebGL1ContextInitCommandBufferResponse(const WebGL1ContextInitCommandBufferResponse &that, bool clone = false)
        : TrCommandBufferSimpleResponse(that, clone)
        , drawingViewport(that.drawingViewport)
        , maxCombinedTextureImageUnits(that.maxCombinedTextureImageUnits)
        , maxCubeMapTextureSize(that.maxCubeMapTextureSize)
        , maxFragmentUniformVectors(that.maxFragmentUniformVectors)
        , maxRenderbufferSize(that.maxRenderbufferSize)
        , maxTextureImageUnits(that.maxTextureImageUnits)
        , maxTextureSize(that.maxTextureSize)
        , maxVaryingVectors(that.maxVaryingVectors)
        , maxVertexAttribs(that.maxVertexAttribs)
        , maxVertexTextureImageUnits(that.maxVertexTextureImageUnits)
        , maxVertexUniformVectors(that.maxVertexUniformVectors)
    {
      if (clone)
      {
        vendor = that.vendor;
        version = that.version;
        renderer = that.renderer;
      }
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

  class WebGL2ContextInitCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<WebGL2ContextInitCommandBufferRequest,
                                            COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_REQ>
  {
    using TrCommandBufferSimpleRequest::TrCommandBufferSimpleRequest;
  };

  class WebGL2ContextInitCommandBufferResponse final
      : public TrCommandBufferSimpleResponse<WebGL2ContextInitCommandBufferResponse>
  {
  public:
    WebGL2ContextInitCommandBufferResponse(WebGL2ContextInitCommandBufferRequest *req)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_RES, req)
    {
    }
    WebGL2ContextInitCommandBufferResponse(const WebGL2ContextInitCommandBufferResponse &that, bool clone = false)
        : TrCommandBufferSimpleResponse(that, clone)
        , max3DTextureSize(that.max3DTextureSize)
        , maxArrayTextureLayers(that.maxArrayTextureLayers)
        , maxColorAttachments(that.maxColorAttachments)
        , maxCombinedUniformBlocks(that.maxCombinedUniformBlocks)
        , maxDrawBuffers(that.maxDrawBuffers)
        , maxElementsIndices(that.maxElementsIndices)
        , maxElementsVertices(that.maxElementsVertices)
        , maxFragmentInputComponents(that.maxFragmentInputComponents)
        , maxFragmentUniformBlocks(that.maxFragmentUniformBlocks)
        , maxFragmentUniformComponents(that.maxFragmentUniformComponents)
        , maxProgramTexelOffset(that.maxProgramTexelOffset)
        , maxSamples(that.maxSamples)
        , maxTransformFeedbackInterleavedComponents(that.maxTransformFeedbackInterleavedComponents)
        , maxTransformFeedbackSeparateAttributes(that.maxTransformFeedbackSeparateAttributes)
        , maxTransformFeedbackSeparateComponents(that.maxTransformFeedbackSeparateComponents)
        , maxUniformBufferBindings(that.maxUniformBufferBindings)
        , maxVaryingComponents(that.maxVaryingComponents)
        , maxVertexOutputComponents(that.maxVertexOutputComponents)
        , maxVertexUniformBlocks(that.maxVertexUniformBlocks)
        , maxVertexUniformComponents(that.maxVertexUniformComponents)
        , minProgramTexelOffset(that.minProgramTexelOffset)
        , maxClientWaitTimeout(that.maxClientWaitTimeout)
        , maxCombinedFragmentUniformComponents(that.maxCombinedFragmentUniformComponents)
        , maxCombinedVertexUniformComponents(that.maxCombinedVertexUniformComponents)
        , maxElementIndex(that.maxElementIndex)
        , maxServerWaitTimeout(that.maxServerWaitTimeout)
        , maxUniformBlockSize(that.maxUniformBlockSize)
        , maxTextureLODBias(that.maxTextureLODBias)
        // Extensions
        , OVR_maxViews(that.OVR_maxViews)
        , maxTextureMaxAnisotropy(that.maxTextureMaxAnisotropy)
    {
    }
    ~WebGL2ContextInitCommandBufferResponse()
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

  public: // Extensions
    int32_t OVR_maxViews;
    float maxTextureMaxAnisotropy;
  };
}
