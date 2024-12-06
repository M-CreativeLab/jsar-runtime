#include "./device.hpp"
#include "./webxr_system.hpp"

using namespace std;

namespace client_xr
{
  bool XRSystem::isSessionSupported(XRSessionMode mode)
  {
    return device_->isSessionSupported(mode);
  }

  shared_ptr<XRSession> XRSystem::requestSession(XRSessionMode mode, std::optional<XRSessionRequestInit> init)
  {
    XRSessionRequestInit options = init.value_or(XRSessionRequestInit::Default());
    std::vector<xr::TrXRFeature> enabledFeatures;
    auto requiredFeatures = options.requiredFeatures;
    auto optionalFeatures = options.optionalFeatures;

    bool requirementsFailed = false;
    for (auto &feature : requiredFeatures)
    {
      if (!device_->supportsFeature(feature))
      {
        std::cout << "required feature not supported: " << xr::to_string(feature) << std::endl;
        requirementsFailed = true;
        break;
      }
      else
      {
        enabledFeatures.push_back(feature);
      }
    }

    if (requirementsFailed)
      throw std::runtime_error("required features are not supported.");

    for (auto &feature : optionalFeatures)
    {
      if (device_->supportsFeature(feature))
        enabledFeatures.push_back(feature);
    }
    auto newSession = XRSession::Make(device_->requestSession(mode, enabledFeatures, init),
                                      shared_from_this());
    newSession->initialize();
    newSession->start();
    return newSession;
  }
}
