#include "navigator.hpp"
#include <thread>

namespace browser
{
  Navigator::Navigator()
      : userAgent_("Mozilla/5.0 (JSAR-Runtime) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36")
      , platform_("JSAR")
      , vendor_("M-CreativeLab")
      , language_("en-US")
      , hardwareConcurrency_(static_cast<int>(std::thread::hardware_concurrency()))
      , onLine_(true)
      , cookieEnabled_(true)
      , doNotTrack_(false)
  {
    // Initialize languages array
    languages_.push_back("en-US");
    languages_.push_back("en");
  }

  Navigator::~Navigator() = default;

  std::string Navigator::GetUserAgent() const
  {
    return userAgent_;
  }

  std::string Navigator::GetPlatform() const
  {
    return platform_;
  }

  std::string Navigator::GetVendor() const
  {
    return vendor_;
  }

  std::string Navigator::GetVendorSub() const
  {
    return "";
  }

  std::string Navigator::GetProduct() const
  {
    return "Gecko";
  }

  std::string Navigator::GetProductSub() const
  {
    return "20030107";
  }

  bool Navigator::IsOnLine() const
  {
    return onLine_;
  }

  bool Navigator::IsCookieEnabled() const
  {
    return cookieEnabled_;
  }

  bool Navigator::IsDoNotTrack() const
  {
    return doNotTrack_;
  }

  int Navigator::GetHardwareConcurrency() const
  {
    return hardwareConcurrency_;
  }

  long long Navigator::GetMaxTouchPoints() const
  {
    return 1; // Default single touch support
  }

  std::string Navigator::GetLanguage() const
  {
    return language_;
  }

  std::vector<std::string> Navigator::GetLanguages() const
  {
    return languages_;
  }

  bool Navigator::IsJavaEnabled() const
  {
    return false; // Java not supported in JSAR Runtime
  }

  std::string Navigator::GetColorScheme() const
  {
    return "light"; // Default color scheme
  }

  bool Navigator::HasWebGL() const
  {
    return true; // WebGL supported
  }

  bool Navigator::HasWebXR() const
  {
    return true; // WebXR supported
  }

  bool Navigator::HasServiceWorker() const
  {
    return true; // Service Workers supported
  }

  bool Navigator::HasGeolocation() const
  {
    return true; // Geolocation API supported
  }

  bool Navigator::HasMediaDevices() const
  {
    return true; // MediaDevices API supported
  }

  bool Navigator::HasPermissions() const
  {
    return true; // Permissions API supported
  }

  bool Navigator::HasBattery() const
  {
    return true; // Battery API supported
  }

  bool Navigator::HasClipboard() const
  {
    return true; // Clipboard API supported
  }

  long long Navigator::GetStorageQuota() const
  {
    return 1024 * 1024 * 1024; // 1GB default storage quota
  }
}