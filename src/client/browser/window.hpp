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
#include <client/cssom/media_queries.hpp>
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
    inline const std::string &name() const { return name_; }
    inline const std::string &origin() const { return origin_; }
    inline const client_cssom::Device &device() const { return device_; }
    inline client_cssom::Device &device() { return device_; }

    inline bool fullscreen() const { return fullscreen_; }
    inline float innerWidth() const { return inner_width_; }
    inline float innerHeight() const { return inner_height_; }
    inline float innerDepth() const { return inner_depth_; }
    inline float outerWidth() const { return outer_width_; }
    inline float outerHeight() const { return outer_height_; }
    inline float outerDepth() const { return outer_depth_; }

    inline float scrollX() const { return scroll_x_; }
    inline float scrollY() const { return scroll_y_; }

    inline float devicePixelRatio() const { return device_.devicePixelRatio(); }
    inline float &devicePixelRatio() { return device_.devicePixelRatio(); }

    inline TrViewport viewport() const
    {
      auto viewport = device_.viewportSize();
      return TrViewport(viewport.x, viewport.y, 0, 0);
    }

    inline std::shared_ptr<Window> self() { return shared_from_this(); }
    inline std::shared_ptr<dom::Document> document() const { return document_; }

  public:
    /**
     * Displays an alert dialog with the specified message.
     *
     * @param message The message to display.
     */
    inline void alert(const std::string &message)
    {
      client_context_->makeRpcCall("window.alert", {message});
    }

    /**
     * Closes the current window.
     */
    inline void close()
    {
      client_context_->makeRpcCall("window.close", {});
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
      client_context_->makeRpcCall("window.open", {url, to_string(target)});
    }

    /**
     * Displays a prompt dialog with the specified message and default value.
     *
     * @param message The message to display.
     * @param defaultValue The default value for the input field.
     */
    inline void prompt(const std::string &message, const std::string &defaultValue)
    {
      client_context_->makeRpcCall("window.prompt", {message, defaultValue});
    }

    /**
     * Gets an object containing the values of all CSS properties of an element, after applying active stylesheets and resolving any basic
     * computation those values may contain.
     *
     * @param elementOrTextNode The element or text node to get the computed style.
     * @param pseudoElt The optional pseudo-element to get the computed style.
     * 
     * @todo Implement the pseudo-element support.
     */
    const client_cssom::ComputedStyle &getComputedStyle(std::shared_ptr<dom::Node> elementOrTextNode,
                                                        std::optional<std::string> pseudoElt = std::nullopt) const;

  private:
    // Configure the document to the window.
    void configureDocument(std::shared_ptr<dom::Document> document)
    {
      assert(is_document_configured_ == false);
      document_ = document;
      is_document_configured_ = true;
    }

  private: // Window properties
    std::string name_ = "";
    std::string origin_ = "";
    client_cssom::Device device_ = client_cssom::Device();
    bool fullscreen_ = false;
    float inner_width_ = client_cssom::ScreenWidth;
    float inner_height_ = client_cssom::ScreenHeight;
    float inner_depth_ = client_cssom::VolumeDepth;
    float outer_width_ = client_cssom::ScreenWidth;
    float outer_height_ = client_cssom::ScreenHeight;
    float outer_depth_ = client_cssom::VolumeDepth;
    float scroll_x_ = 0.0f;
    float scroll_y_ = 0.0f;
    std::shared_ptr<dom::Document> document_;

  private:
    TrClientContextPerProcess *client_context_; // The client context for making RPC calls
    bool is_document_configured_ = false;
  };
} // namespace browser
