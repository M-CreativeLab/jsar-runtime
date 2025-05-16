#pragma once

#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <common/utility.hpp>
#include <common/viewport.hpp>
#include <common/events_v2/event_target.hpp>
#include <client/classes.hpp>
#include <client/per_process.hpp>
#include <client/cssom/units.hpp>
#include <client/cssom/css_style_declaration.hpp>
#include <client/cssom/computed_style.hpp>
#include <client/dom/dom_event_target.hpp>

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
  class Window : public dom::DOMEventTarget,
                 public std::enable_shared_from_this<Window>
  {
    friend class dom::Document;

  public:
    Window(TrClientContextPerProcess *clientContext = TrClientContextPerProcess::Get());

  public:
    /**
     * @returns the name of the window.
     */
    inline const std::string &name() const { return name_; }

    /**
     * @returns the origin of the window.
     */
    inline const std::string &origin() const { return origin_; }

    /**
     * @returns true if the window is in fullscreen mode.
     */
    inline bool fullscreen() const { return fullscreen_; }

    /**
     * @returns the inner width of the window.
     */
    inline float innerWidth() const { return innerWidth_; }

    /**
     * @returns the inner height of the window.
     */
    inline float innerHeight() const { return innerHeight_; }

    /**
     * @returns the inner depth of the window.
     */
    inline float innerDepth() const { return innerDepth_; }

    /**
     * @returns the outer width of the window.
     */
    inline float outerWidth() const { return outerWidth_; }

    /**
     * @returns the outer height of the window.
     */
    inline float outerHeight() const { return outerHeight_; }

    /**
     * @returns the outer depth of the window.
     */
    inline float outerDepth() const { return outerDepth_; }

    /**
     * @returns the horizontal scroll position of the window.
     */
    inline float scrollX() const { return scrollX_; }

    /**
     * @returns the vertical scroll position of the window.
     */
    inline float scrollY() const { return scrollY_; }

    /**
     * @returns the device pixel ratio of the window.
     */
    inline float devicePixelRatio() const { return devicePixelRatio_; }
    inline float &devicePixelRatio() { return devicePixelRatio_; }

    /**
     * @returns the viewport of the window.
     */
    inline TrViewport viewport() const
    {
      return TrViewport(innerWidth_, innerHeight_, 0, 0);
    }

    /**
     * @returns the shared pointer to the window.
     */
    inline std::shared_ptr<Window> self() { return shared_from_this(); }

    /**
     * @returns the document of the window.
     */
    inline std::shared_ptr<dom::Document> document() const { return document_; }

  public:
    /**
     * Displays an alert dialog with the specified message.
     *
     * @param message The message to display.
     */
    inline void alert(const std::string &message)
    {
      clientContext_->makeRpcCall("window.alert", {message});
    }

    /**
     * Closes the current window.
     */
    inline void close()
    {
      clientContext_->makeRpcCall("window.close", {});
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
      clientContext_->makeRpcCall("window.open", {url, to_string(target)});
    }

    /**
     * Displays a prompt dialog with the specified message and default value.
     *
     * @param message The message to display.
     * @param defaultValue The default value for the input field.
     */
    inline void prompt(const std::string &message, const std::string &defaultValue)
    {
      clientContext_->makeRpcCall("window.prompt", {message, defaultValue});
    }

    /**
     * Gets an object containing the values of all CSS properties of an element, after applying active stylesheets and resolving any basic
     * computation those values may contain.
     *
     * @param element The element to get the computed style.
     * @param pseudoElt The optional pseudo-element to get the computed style.
     */
    const client_cssom::ComputedStyle &getComputedStyle(std::shared_ptr<dom::Element> element,
                                                        std::optional<std::string> pseudoElt = std::nullopt) const;

  private:
    // Configure the document to the window.
    void configureDocument(std::shared_ptr<dom::Document> document)
    {
      assert(isDocumentConfigured_ == false);
      document_ = document;
      isDocumentConfigured_ = true;
    }

  private: // Window properties
    std::string name_ = "";
    std::string origin_ = "";
    bool fullscreen_ = false;
    float innerWidth_ = client_cssom::ScreenWidth;
    float innerHeight_ = client_cssom::ScreenHeight;
    float innerDepth_ = client_cssom::VolumeDepth;
    float outerWidth_ = client_cssom::ScreenWidth;
    float outerHeight_ = client_cssom::ScreenHeight;
    float outerDepth_ = client_cssom::VolumeDepth;
    float scrollX_ = 0.0f;
    float scrollY_ = 0.0f;
    float devicePixelRatio_ = 1.5f;
    std::shared_ptr<dom::Document> document_;

  private:
    TrClientContextPerProcess *clientContext_; // The client context for making RPC calls
    bool isDocumentConfigured_ = false;
  };
} // namespace browser
