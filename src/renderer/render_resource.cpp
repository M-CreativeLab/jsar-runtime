#include <renderer/render_resource.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  TrRenderResource::TrRenderResource(Ref<GPUDeviceBase> device)
      : device_(device)
      , pipelines_()
      , buffers_()
      , textures_()
  {
  }

  void TrRenderResource::receiveIncomingRequest(const TrCommandBufferRequest &request)
  {
  }

  Ref<GPUPipelineBase> TrRenderResource::getPipeline(int id)
  {
    return pipelines_.at(id);
  }

  Ref<GPUBufferBase> TrRenderResource::getBuffer(int id)
  {
    return buffers_.at(id);
  }

  Ref<GPUTextureBase> TrRenderResource::getTexture(int id)
  {
    return textures_.at(id);
  }
}
