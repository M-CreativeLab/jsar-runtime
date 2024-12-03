#pragma once

#include <memory>
#include "../per_process.hpp"
#include "./common.hpp"

namespace client_xr
{
  class XRDeviceClient
  {
  public:
    /**
     * Create a new instance of the XRDeviceClient.
     * 
     * @param clientContext The client context to use.
     * @returns The new instance of the XRDeviceClient.
     */
    static std::shared_ptr<XRDeviceClient> Make(TrClientContextPerProcess *clientContext)
    {
      return std::make_shared<XRDeviceClient>(clientContext);
    }

  public:
    explicit XRDeviceClient(TrClientContextPerProcess *clientContext)
        : clientContext(clientContext)
    {
    }

  public:
    bool isSessionSupported(XRSessionMode mode);
    bool supportsReferenceSpaceType(XRReferenceSpaceType referenceSpaceType);
    // void requestFrame(XRFrameCallback callback, void *context);
    // bool startFrame(xr::TrXRFrameRequest *frameRequest);
    // bool endFrame(xr::TrXRFrameRequest *frameRequest);
    // TrViewport getViewport(uint32_t viewIndex);
    // xr::TrDeviceInit &getDeviceInit();

  private:
    TrClientContextPerProcess *clientContext = nullptr;
    int requestTimeout = 1000;
  };
}
