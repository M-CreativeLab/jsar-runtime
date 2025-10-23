#pragma once

#include <string>
#include <memory>
#include <optional>
#include <client/scripting_base/v8_object_holder.hpp>
#include <client/dom/dom_event_target.hpp>

namespace dom
{
  /**
   * Permission state for notifications.
   * 
   * @see https://www.w3.org/TR/notifications/#permission
   */
  enum class NotificationPermission
  {
    kDefault,
    kDenied,
    kGranted
  };

  /**
   * Convert permission enum to string.
   */
  inline std::string NotificationPermissionToString(NotificationPermission permission)
  {
    switch (permission)
    {
    case NotificationPermission::kDefault:
      return "default";
    case NotificationPermission::kDenied:
      return "denied";
    case NotificationPermission::kGranted:
      return "granted";
    default:
      return "default";
    }
  }

  /**
   * Convert string to permission enum.
   */
  inline NotificationPermission StringToNotificationPermission(const std::string &permission)
  {
    if (permission == "granted")
      return NotificationPermission::kGranted;
    if (permission == "denied")
      return NotificationPermission::kDenied;
    return NotificationPermission::kDefault;
  }

  /**
   * Direction for notification text display.
   */
  enum class NotificationDirection
  {
    kAuto,
    kLtr,
    kRtl
  };

  /**
   * Convert direction enum to string.
   */
  inline std::string NotificationDirectionToString(NotificationDirection direction)
  {
    switch (direction)
    {
    case NotificationDirection::kAuto:
      return "auto";
    case NotificationDirection::kLtr:
      return "ltr";
    case NotificationDirection::kRtl:
      return "rtl";
    default:
      return "auto";
    }
  }

  /**
   * Convert string to direction enum.
   */
  inline NotificationDirection StringToNotificationDirection(const std::string &direction)
  {
    if (direction == "ltr")
      return NotificationDirection::kLtr;
    if (direction == "rtl")
      return NotificationDirection::kRtl;
    return NotificationDirection::kAuto;
  }

  /**
   * Options for creating a Notification.
   * 
   * @see https://www.w3.org/TR/notifications/#dictdef-notificationoptions
   */
  struct NotificationOptions
  {
    NotificationDirection dir = NotificationDirection::kAuto;
    std::string lang;
    std::string body;
    std::string tag;
    std::string icon;
    std::string badge;
    std::string sound;
    bool renotify = false;
    bool requireInteraction = false;
    bool silent = false;
    // data is represented as a void pointer for now, can be extended later
    void *data = nullptr;
  };

  /**
   * The Notification interface represents a system notification displayed to the user.
   * 
   * This class implements the Web Notification API as defined in the W3C specification.
   * It allows web content to display notifications to the user outside of the web page.
   * 
   * @see https://www.w3.org/TR/notifications/
   * @see https://developer.mozilla.org/en-US/docs/Web/API/Notification
   */
  class Notification : public DOMEventTarget
  {
  public:
    /**
     * Constructor for Notification.
     * 
     * @param title The title of the notification
     * @param options Options for configuring the notification
     */
    Notification(const std::string &title, const NotificationOptions &options = NotificationOptions());

    /**
     * Destructor.
     */
    virtual ~Notification() = default;

  public:
    // Static methods and properties

    /**
     * Get the current permission state for notifications.
     * 
     * @return The current permission state
     */
    static NotificationPermission permission();

    /**
     * Request permission to display notifications.
     * 
     * Note: This is a stub implementation. In a real implementation, this would
     * trigger a permission dialog and return a promise.
     * 
     * @return The new permission state
     */
    static NotificationPermission requestPermission();

  public:
    // Instance properties (read-only)

    /**
     * Get the title of the notification.
     */
    inline const std::string &title() const
    {
      return title_;
    }

    /**
     * Get the direction of the notification.
     */
    inline NotificationDirection dir() const
    {
      return dir_;
    }

    /**
     * Get the language of the notification.
     */
    inline const std::string &lang() const
    {
      return lang_;
    }

    /**
     * Get the body text of the notification.
     */
    inline const std::string &body() const
    {
      return body_;
    }

    /**
     * Get the tag of the notification.
     */
    inline const std::string &tag() const
    {
      return tag_;
    }

    /**
     * Get the icon URL of the notification.
     */
    inline const std::string &icon() const
    {
      return icon_;
    }

    /**
     * Get the badge URL of the notification.
     */
    inline const std::string &badge() const
    {
      return badge_;
    }

    /**
     * Get the sound URL of the notification.
     */
    inline const std::string &sound() const
    {
      return sound_;
    }

    /**
     * Get whether the notification should re-notify if replaced.
     */
    inline bool renotify() const
    {
      return renotify_;
    }

    /**
     * Get whether the notification requires user interaction to dismiss.
     */
    inline bool requireInteraction() const
    {
      return require_interaction_;
    }

    /**
     * Get whether the notification should be silent.
     */
    inline bool silent() const
    {
      return silent_;
    }

    /**
     * Get the custom data associated with the notification.
     */
    inline void *data() const
    {
      return data_;
    }

  public:
    // Instance methods

    /**
     * Close the notification programmatically.
     */
    void close();

  public:
    // Event handlers (to be implemented via EventTarget)
    // - onshow: fired when the notification is displayed
    // - onclick: fired when the notification is clicked
    // - onclose: fired when the notification is closed
    // - onerror: fired when an error occurs

  protected:
    /**
     * Show the notification (platform-specific implementation).
     */
    virtual void show();

  private:
    std::string title_;
    NotificationDirection dir_;
    std::string lang_;
    std::string body_;
    std::string tag_;
    std::string icon_;
    std::string badge_;
    std::string sound_;
    bool renotify_;
    bool require_interaction_;
    bool silent_;
    void *data_;
    bool is_closed_;

    // Static permission state (stub implementation)
    static NotificationPermission permission_;
  };
}
