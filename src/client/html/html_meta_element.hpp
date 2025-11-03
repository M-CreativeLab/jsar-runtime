#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include "./html_element.hpp"

namespace endor
{
  namespace dom
  {
    /**
   * Parsed viewport meta tag values following MDN specification
   * https://developer.mozilla.org/en-US/docs/Web/HTML/Reference/Elements/meta/name/viewport
   */
    struct ViewportMeta
    {
      std::optional<float> width;         // Width in pixels or "device-width"
      std::optional<float> height;        // Height in pixels or "device-height"
      std::optional<float> initial_scale; // Initial zoom factor (0.1 to 10.0)
      std::optional<float> minimum_scale; // Minimum zoom factor (0.1 to 10.0)
      std::optional<float> maximum_scale; // Maximum zoom factor (0.1 to 10.0)
      std::optional<bool> user_scalable;  // Whether user can zoom (yes/no)

      bool device_width = false;  // True if width="device-width"
      bool device_height = false; // True if height="device-height"
    };

    class HTMLMetaElement : public HTMLElement
    {
    public:
      using HTMLElement::HTMLElement;
      HTMLMetaElement(std::shared_ptr<Document> ownerDocument)
          : HTMLElement("META", ownerDocument)
      {
      }

    public:
      string getContent() const
      {
        return getAttribute("content");
      }
      void setContent(string value)
      {
        setAttribute("content", value);
        onContentChanged();
      }
      string getHttpEquiv() const
      {
        return getAttribute("http-equiv");
      }
      void setHttpEquiv(string value)
      {
        setAttribute("http-equiv", value);
      }
      string getMedia() const
      {
        return getAttribute("media");
      }
      void setMedia(string value)
      {
        setAttribute("media", value);
      }
      string getName() const
      {
        return getAttribute("name");
      }
      void setName(string value)
      {
        setAttribute("name", value);
        onNameChanged();
      }

      /**
     * Check if this is a viewport meta tag
     */
      bool isViewportMeta() const
      {
        return getName() == "viewport";
      }

      /**
     * Parse viewport meta content according to MDN specification
     * Returns nullopt if this is not a viewport meta tag or parsing fails
     */
      std::optional<ViewportMeta> parseViewportMeta() const;

    private:
      /**
     * Called when content attribute changes - notifies document if this is viewport meta
     */
      void onContentChanged();

      /**
     * Called when name attribute changes - notifies document if becoming/leaving viewport meta
     */
      void onNameChanged();

      /**
     * Parse a single viewport directive (e.g., "width=640" or "user-scalable=no")
     */
      void parseViewportDirective(const std::string &directive, ViewportMeta &meta) const;

      /**
     * Parse a numeric value, handling special keywords
     */
      std::optional<float> parseNumericValue(const std::string &value) const;

      /**
     * Parse a boolean value (yes/no, 1/0)
     */
      std::optional<bool> parseBooleanValue(const std::string &value) const;

      /**
     * Trim whitespace from string
     */
      std::string trim(const std::string &str) const;
    };
  }
} // namespace endor
