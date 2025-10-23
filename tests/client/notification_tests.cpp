#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/dom/notification.hpp>
#include <memory>

using namespace dom;

TEST_CASE("Notification constructor", "[Notification]")
{
  NotificationOptions options;
  options.body = "Test notification body";
  options.icon = "test-icon.png";
  options.tag = "test-tag";

  auto notification = std::make_shared<Notification>("Test Notification", options);

  REQUIRE(notification->title() == "Test Notification");
  REQUIRE(notification->body() == "Test notification body");
  REQUIRE(notification->icon() == "test-icon.png");
  REQUIRE(notification->tag() == "test-tag");
}

TEST_CASE("Notification default options", "[Notification]")
{
  auto notification = std::make_shared<Notification>("Simple Notification");

  REQUIRE(notification->title() == "Simple Notification");
  REQUIRE(notification->body() == "");
  REQUIRE(notification->icon() == "");
  REQUIRE(notification->tag() == "");
  REQUIRE(notification->dir() == NotificationDirection::kAuto);
  REQUIRE(notification->silent() == false);
  REQUIRE(notification->requireInteraction() == false);
}

TEST_CASE("Notification permission", "[Notification]")
{
  // Initially should be default
  auto permission = Notification::permission();
  REQUIRE(permission == NotificationPermission::kDefault);

  // Request permission (stub implementation grants it)
  auto newPermission = Notification::requestPermission();
  REQUIRE(newPermission == NotificationPermission::kGranted);

  // Permission should now be granted
  auto currentPermission = Notification::permission();
  REQUIRE(currentPermission == NotificationPermission::kGranted);
}

TEST_CASE("Notification close", "[Notification]")
{
  auto notification = std::make_shared<Notification>("Closeable Notification");

  // Close the notification
  notification->close();
  // Note: We can't directly test the closed state as it's private
  // In a real implementation, this would fire events
}

TEST_CASE("Notification direction enum conversion", "[Notification]")
{
  REQUIRE(NotificationDirectionToString(NotificationDirection::kAuto) == "auto");
  REQUIRE(NotificationDirectionToString(NotificationDirection::kLtr) == "ltr");
  REQUIRE(NotificationDirectionToString(NotificationDirection::kRtl) == "rtl");

  REQUIRE(StringToNotificationDirection("auto") == NotificationDirection::kAuto);
  REQUIRE(StringToNotificationDirection("ltr") == NotificationDirection::kLtr);
  REQUIRE(StringToNotificationDirection("rtl") == NotificationDirection::kRtl);
  REQUIRE(StringToNotificationDirection("invalid") == NotificationDirection::kAuto);
}

TEST_CASE("Notification permission enum conversion", "[Notification]")
{
  REQUIRE(NotificationPermissionToString(NotificationPermission::kDefault) == "default");
  REQUIRE(NotificationPermissionToString(NotificationPermission::kGranted) == "granted");
  REQUIRE(NotificationPermissionToString(NotificationPermission::kDenied) == "denied");

  REQUIRE(StringToNotificationPermission("default") == NotificationPermission::kDefault);
  REQUIRE(StringToNotificationPermission("granted") == NotificationPermission::kGranted);
  REQUIRE(StringToNotificationPermission("denied") == NotificationPermission::kDenied);
  REQUIRE(StringToNotificationPermission("invalid") == NotificationPermission::kDefault);
}

TEST_CASE("Notification with all options", "[Notification]")
{
  NotificationOptions options;
  options.dir = NotificationDirection::kRtl;
  options.lang = "en-US";
  options.body = "Full notification";
  options.tag = "full-tag";
  options.icon = "icon.png";
  options.badge = "badge.png";
  options.sound = "sound.mp3";
  options.renotify = true;
  options.requireInteraction = true;
  options.silent = true;

  auto notification = std::make_shared<Notification>("Full Notification", options);

  REQUIRE(notification->title() == "Full Notification");
  REQUIRE(notification->dir() == NotificationDirection::kRtl);
  REQUIRE(notification->lang() == "en-US");
  REQUIRE(notification->body() == "Full notification");
  REQUIRE(notification->tag() == "full-tag");
  REQUIRE(notification->icon() == "icon.png");
  REQUIRE(notification->badge() == "badge.png");
  REQUIRE(notification->sound() == "sound.mp3");
  REQUIRE(notification->renotify() == true);
  REQUIRE(notification->requireInteraction() == true);
  REQUIRE(notification->silent() == true);
}
