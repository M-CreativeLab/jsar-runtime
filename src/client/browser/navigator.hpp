#pragma once

#include <memory>
#include <string>
#include <vector>
#include <client/scripting_base/v8_object_holder.hpp>

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
    Navigator();
    ~Navigator();

    // Browser identification
    std::string GetUserAgent() const;
    std::string GetPlatform() const;
    std::string GetVendor() const;
    std::string GetVendorSub() const;
    std::string GetProduct() const;
    std::string GetProductSub() const;

    // Browser capabilities
    bool IsOnLine() const;
    bool IsCookieEnabled() const;
    bool IsDoNotTrack() const;
    int GetHardwareConcurrency() const;
    long long GetMaxTouchPoints() const;

    // Language support
    std::string GetLanguage() const;
    std::vector<std::string> GetLanguages() const;

    // User preferences
    bool IsJavaEnabled() const;
    std::string GetColorScheme() const;

    // Platform features
    bool HasWebGL() const;
    bool HasWebXR() const;
    bool HasServiceWorker() const;
    bool HasGeolocation() const;
    bool HasMediaDevices() const;
    bool HasPermissions() const;

    // Battery API
    bool HasBattery() const;

    // Clipboard API
    bool HasClipboard() const;

    // Storage
    long long GetStorageQuota() const;

  private:
    std::string userAgent_;
    std::string platform_;
    std::string vendor_;
    std::string language_;
    std::vector<std::string> languages_;
    int hardwareConcurrency_;
    bool onLine_;
    bool cookieEnabled_;
    bool doNotTrack_;
  };
}