#pragma once

#include <memory>
#include <string>
#include <vector>
#include <client/per_process.hpp>
#include <client/scripting_base/v8_object_holder.hpp>
#include <client/graphics/webgl_context.hpp>
#include <client/xr/webxr_system.hpp>

namespace endor
{
  namespace browser
  {
    /**
   * Navigator class implementing the Web API Navigator interface.
   * 
   * Provides information about the browser and system capabilities,
   * following the MDN Web API specification:
   * https://developer.mozilla.org/en-US/docs/Web/API/Navigator
   */
    class Navigator : public scripting_base::JSObjectHolder
    {
    public:
      Navigator(TrClientContextPerProcess *client_context);
      ~Navigator() = default;

      // Browser identification
      std::string userAgent() const;
      std::string platform() const;
      std::string vendor() const;
      std::string vendorSub() const;
      std::string product() const;
      std::string productSub() const;

      // Browser capabilities
      bool isOnline() const;
      bool isCookieEnabled() const;
      bool isDoNotTrack() const;
      int hardwareConcurrency() const;
      long long maxTouchPoints() const;

      // Language support
      std::string language() const;
      std::vector<std::string> languages() const;

      // User preferences
      bool isJavaEnabled() const;
      std::string colorScheme() const;

      // Platform features
      bool hasWebGL() const;
      bool hasWebXR() const;
      bool hasServiceWorker() const;
      bool hasGeolocation() const;
      bool hasMediaDevices() const;
      bool hasPermissions() const;

      // Battery API
      bool hasBattery() const;

      // Clipboard API
      bool hasClipboard() const;

      // Storage
      long long getStorageQuota() const;

      // Components
      inline std::shared_ptr<client_graphics::WebGL2Context> getWebGLContext() const
      {
        return gl_context_;
      }
      inline std::shared_ptr<client_xr::XRSystem> getXRSystem() const
      {
        return xr_system_;
      }

    private:
      void initXRSystem(TrClientContextPerProcess *client_context);

    private:
      std::string user_agent_;
      std::string platform_;
      std::string vendor_;
      std::string language_;
      std::vector<std::string> languages_;
      int hardware_concurrency_;
      bool online_;
      bool cookie_enabled_;
      bool do_not_track_;

      // Components
      std::shared_ptr<client_graphics::WebGL2Context> gl_context_;
      std::shared_ptr<client_xr::XRSystem> xr_system_;
    };
  }
} // namespace endor
