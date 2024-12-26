#pragma once

#include <optional>
#include <string>
#include <vector>
#include <memory>
#include "common/utility.hpp"
#include "common/events_v2/event_target.hpp"
#include "../per_process.hpp"
#include "../dom/dom_event_target.hpp"

namespace browser
{
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

  class WindowFeatures final
  {
  public:
    WindowFeatures() = default;

  public:
    bool popup = false;
    bool noopener = false;
    bool noreferrer = false;
    std::optional<uint32_t> width = std::nullopt;
    std::optional<uint32_t> height = std::nullopt;
    std::optional<uint32_t> depth = std::nullopt;
    std::optional<uint32_t> left = std::nullopt;
    std::optional<uint32_t> top = std::nullopt;
  };

  class Window : public dom::DOMEventTarget
  {
  public:
    Window() : dom::DOMEventTarget(),
               clientContext(TrClientContextPerProcess::Get())
    {
    }

  public:
    inline void alert(string message)
    {
      clientContext->makeRpcCall("window.alert", {message});
    }
    inline void close()
    {
      clientContext->makeRpcCall("window.close", {});
    }
    inline void open(string url, WindowTarget target, WindowFeatures features = WindowFeatures())
    {
      clientContext->makeRpcCall("window.open", {url, to_string(target)});
    }
    inline void prompt(string message, string defaultValue)
    {
      clientContext->makeRpcCall("window.prompt", {message, defaultValue});
    }

  private:
    TrClientContextPerProcess *clientContext;
  };
}
