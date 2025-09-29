#include <thread>
#include <client/xr/device.hpp>
#include "./navigator.hpp"

using namespace std;

namespace browser
{
  Navigator::Navigator(TrClientContextPerProcess *client_context)
      : user_agent_("Mozilla/5.0 (JSAR-Runtime) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36")
      , platform_("JSAR")
      , vendor_("M-CreativeLab")
      , language_("en-US")
      , hardware_concurrency_(static_cast<int>(thread::hardware_concurrency()))
      , online_(true)
      , cookie_enabled_(true)
      , do_not_track_(false)
      , gl_context_(client_context->createHostWebGLContext())
      , xr_system_(nullptr)
  {
    // If the scripting event loop is already ready, initialize the XRSystem immediately
    if (client_context->isScriptingEventLoopReady())
    {
      initXRSystem(client_context);
    }
    else
    {
      client_context->addEventListener(
        TrClientContextEventType::ScriptingEventLoopReady,
        [this](auto _type, auto _event)
        {
          initXRSystem(TrClientContextPerProcess::Get());
        });
    }
  }

  string Navigator::userAgent() const
  {
    return user_agent_;
  }

  string Navigator::platform() const
  {
    return platform_;
  }

  string Navigator::vendor() const
  {
    return vendor_;
  }

  string Navigator::vendorSub() const
  {
    return "";
  }

  string Navigator::product() const
  {
    return "JSAR";
  }

  string Navigator::productSub() const
  {
    return "20030107";
  }

  bool Navigator::isOnline() const
  {
    return online_;
  }

  bool Navigator::isCookieEnabled() const
  {
    return cookie_enabled_;
  }

  bool Navigator::isDoNotTrack() const
  {
    return do_not_track_;
  }

  int Navigator::hardwareConcurrency() const
  {
    return hardware_concurrency_;
  }

  long long Navigator::maxTouchPoints() const
  {
    return 1; // Default single touch support
  }

  string Navigator::language() const
  {
    return language_;
  }

  vector<string> Navigator::languages() const
  {
    return languages_;
  }

  bool Navigator::isJavaEnabled() const
  {
    return false; // Java not supported in JSAR Runtime
  }

  string Navigator::colorScheme() const
  {
    return "light"; // Default color scheme
  }

  bool Navigator::hasWebGL() const
  {
    return gl_context_ != nullptr;
  }

  bool Navigator::hasWebXR() const
  {
    return xr_system_ != nullptr;
  }

  bool Navigator::hasServiceWorker() const
  {
    return true; // Service Workers supported
  }

  bool Navigator::hasGeolocation() const
  {
    return true; // Geolocation API supported
  }

  bool Navigator::hasMediaDevices() const
  {
    return true; // MediaDevices API supported
  }

  bool Navigator::hasPermissions() const
  {
    return true; // Permissions API supported
  }

  bool Navigator::hasBattery() const
  {
    return true; // Battery API supported
  }

  bool Navigator::hasClipboard() const
  {
    return true; // Clipboard API supported
  }

  long long Navigator::getStorageQuota() const
  {
    return 1024 * 1024 * 1024; // 1GB default storage quota
  }

  void Navigator::initXRSystem(TrClientContextPerProcess *client_context)
  {
    auto xrDevice = client_context->getXRDeviceClient();
    uv_loop_t *loop = client_context->getScriptingEventLoop();
    assert(loop != nullptr && "Event loop must be ready when creating XRSystem.");

    xr_system_ = xrDevice->createXRSystem(loop);
  }
}
