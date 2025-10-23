#include "notification.hpp"
#include <iostream>

namespace dom
{
  // Initialize static permission state to default
  NotificationPermission Notification::permission_ = NotificationPermission::kDefault;

  Notification::Notification(const std::string &title, const NotificationOptions &options)
      : DOMEventTarget()
      , title_(title)
      , dir_(options.dir)
      , lang_(options.lang)
      , body_(options.body)
      , tag_(options.tag)
      , icon_(options.icon)
      , badge_(options.badge)
      , sound_(options.sound)
      , renotify_(options.renotify)
      , require_interaction_(options.requireInteraction)
      , silent_(options.silent)
      , data_(options.data)
      , is_closed_(false)
  {
    // Automatically show the notification when created
    show();
  }

  NotificationPermission Notification::permission()
  {
    return permission_;
  }

  NotificationPermission Notification::requestPermission()
  {
    // Stub implementation: automatically grant permission
    // In a real implementation, this would:
    // 1. Check if permission was already granted/denied
    // 2. Show a permission dialog to the user
    // 3. Return a promise that resolves with the permission state
    permission_ = NotificationPermission::kGranted;
    return permission_;
  }

  void Notification::close()
  {
    if (!is_closed_)
    {
      is_closed_ = true;

      // TODO: Fire 'close' event
      // This would require creating and dispatching a notification event
      // For now, this is a stub
    }
  }

  void Notification::show()
  {
    if (is_closed_)
    {
      return;
    }

    // Check permission
    if (permission_ != NotificationPermission::kGranted)
    {
      // TODO: Fire 'error' event
      std::cerr << "Notification permission not granted" << std::endl;
      return;
    }

    // Platform-specific notification display
    // TODO: Implement platform-specific notification APIs:
    // - macOS: NSUserNotification
    // - Windows: Toast Notification
    // - Linux: libnotify
    // - XR: JSAR internal 3D UI

    // For now, just log to console
    std::cout << "Notification: " << title_;
    if (!body_.empty())
    {
      std::cout << " - " << body_;
    }
    std::cout << std::endl;

    // TODO: Fire 'show' event
    // This would require creating and dispatching a notification event
  }
}
