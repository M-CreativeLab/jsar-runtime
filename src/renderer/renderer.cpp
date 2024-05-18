#include "renderer.hpp"

namespace renderer
{
  TrRenderer::TrRenderer()
  {
  }

  TrRenderer::~TrRenderer()
  {
  }

  void TrRenderer::initialize()
  {
  }

  void TrRenderer::tickOnAnimationFrame()
  {
    // accept
    auto newClient = animationFrameChanServer->accept();
    if (newClient != nullptr)
      animationFrameChanSenders.push_back(new ipc::TrChannelSender<AnimationFrameRequest>(newClient));

    // Send request to all live clients
    AnimationFrameRequest request;
    for (auto sender : animationFrameChanSenders)
      sender->send(request);
  }
}
