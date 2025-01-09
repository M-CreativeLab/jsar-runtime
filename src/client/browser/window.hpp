#pragma once

#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <common/utility.hpp>
#include <common/events_v2/event_target.hpp>

#include "../per_process.hpp"
#include "../dom/dom_event_target.hpp"

namespace browser
{
  /**
   * @enum WindowTarget
   * The `WindowTarget` enum represents the target browsing context for opening a URL.
   */
  enum class WindowTarget
  {
    /**
     * Open the URL in the current browsing context.
     */
    Self,
    /**
     * Open the URL in a new browsing context, it will create a new volume to render.
     */
    Blank,
    /**
     * Open the URL in a new external browsing context, namely open the page in the classic Web browser in the system.
     */
    BlankClassic,
    /**
     * Open the URL in the parent browsing context, if the parent is from external classic browser, it will open the URL in the classic browser.
     */
    Parent,
    /**
     * The topmost browsing context.
     */
    Top
  };

  /**
   * Convert the window target to the string.
   *
   * @param target The window target.
   * @returns The string representation of the window target.
   * @throws std::invalid_argument if the target is invalid.
   */
  inline std::string to_string(WindowTarget target)
  {
    switch (target)
    {
    case WindowTarget::Self:
      return "_self";
    case WindowTarget::Blank:
      return "_blank";
    case WindowTarget::BlankClassic:
      return "_blankClassic";
    case WindowTarget::Parent:
      return "_parent";
    case WindowTarget::Top:
      return "_top";
    default:
      throw std::invalid_argument("Invalid window open target: " + std::to_string(static_cast<int>(target)));
    }
  }

  /**
   * @class WindowFeatures
   * The `WindowFeatures` class represents the features of a new window.
   */
  class WindowFeatures final
  {
  public:
    WindowFeatures() = default;

  public:
    bool popup = false;             // Whether the window should be opened as a popup
    bool noopener = false;          // Whether the opener should be set to null
    bool noreferrer = false;        // Whether the referrer should be omitted
    std::optional<uint32_t> width;  // The width of the window
    std::optional<uint32_t> height; // The height of the window
    std::optional<uint32_t> depth;  // The depth of the window
    std::optional<uint32_t> left;   // The left position of the window
    std::optional<uint32_t> top;    // The top position of the window
  };

  /**
   * @class Window
   * The `Window` class represents a browser window and provides methods to interact with it.
   */
  class Window : public dom::DOMEventTarget
  {
  public:
    Window() : dom::DOMEventTarget(),
               clientContext(TrClientContextPerProcess::Get())
    {
    }

  public:
    /**
     * Displays an alert dialog with the specified message.
     *
     * @param message The message to display.
     */
    inline void alert(const std::string &message)
    {
      clientContext->makeRpcCall("window.alert", {message});
    }

    /**
     * Closes the current window.
     */
    inline void close()
    {
      clientContext->makeRpcCall("window.close", {});
    }

    /**
     * Opens a new window with the specified URL and target.
     *
     * @param url The URL to open.
     * @param target The target browsing context.
     * @param features The features of the new window.
     */
    inline void open(const std::string &url, WindowTarget target,
                     const WindowFeatures &features = WindowFeatures())
    {
      clientContext->makeRpcCall("window.open", {url, to_string(target)});
    }

    /**
     * Displays a prompt dialog with the specified message and default value.
     *
     * @param message The message to display.
     * @param defaultValue The default value for the input field.
     */
    inline void prompt(const std::string &message, const std::string &defaultValue)
    {
      clientContext->makeRpcCall("window.prompt", {message, defaultValue});
    }

  private:
    TrClientContextPerProcess *clientContext; // The client context for making RPC calls
  };
} // namespace browser
