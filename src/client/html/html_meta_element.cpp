#include "./html_meta_element.hpp"
#include <client/dom/document.hpp>
#include <algorithm>
#include <sstream>

namespace dom
{
  std::optional<ViewportMeta> HTMLMetaElement::parseViewportMeta() const
  {
    if (!isViewportMeta())
      return std::nullopt;

    std::string content = getContent();
    if (content.empty())
      return std::nullopt;

    ViewportMeta meta;

    // Split content by commas and parse each directive
    std::stringstream ss(content);
    std::string directive;

    while (std::getline(ss, directive, ','))
    {
      parseViewportDirective(directive, meta);
    }

    return meta;
  }

  void HTMLMetaElement::parseViewportDirective(const std::string &directive, ViewportMeta &meta) const
  {
    std::string trimmed = trim(directive);
    if (trimmed.empty())
      return;

    // Find the '=' separator
    size_t eq_pos = trimmed.find('=');
    if (eq_pos == std::string::npos)
      return;

    std::string key = trim(trimmed.substr(0, eq_pos));
    std::string value = trim(trimmed.substr(eq_pos + 1));

    if (key.empty() || value.empty())
      return;

    // Parse based on key
    if (key == "width")
    {
      if (value == "device-width")
      {
        meta.device_width = true;
      }
      else
      {
        meta.width = parseNumericValue(value);
      }
    }
    else if (key == "height")
    {
      if (value == "device-height")
      {
        meta.device_height = true;
      }
      else
      {
        meta.height = parseNumericValue(value);
      }
    }
    else if (key == "initial-scale")
    {
      meta.initial_scale = parseNumericValue(value);
      // Clamp to valid range (0.1 to 10.0)
      if (meta.initial_scale && (*meta.initial_scale < 0.1f || *meta.initial_scale > 10.0f))
        meta.initial_scale = std::nullopt;
    }
    else if (key == "minimum-scale")
    {
      meta.minimum_scale = parseNumericValue(value);
      // Clamp to valid range (0.1 to 10.0)
      if (meta.minimum_scale && (*meta.minimum_scale < 0.1f || *meta.minimum_scale > 10.0f))
        meta.minimum_scale = std::nullopt;
    }
    else if (key == "maximum-scale")
    {
      meta.maximum_scale = parseNumericValue(value);
      // Clamp to valid range (0.1 to 10.0)
      if (meta.maximum_scale && (*meta.maximum_scale < 0.1f || *meta.maximum_scale > 10.0f))
        meta.maximum_scale = std::nullopt;
    }
    else if (key == "user-scalable")
    {
      meta.user_scalable = parseBooleanValue(value);
    }
  }

  std::optional<float> HTMLMetaElement::parseNumericValue(const std::string &value) const
  {
    try
    {
      float result = std::stof(value);
      if (std::isfinite(result) && result >= 0)
        return result;
    }
    catch (const std::exception &)
    {
      // Failed to parse as float
    }
    return std::nullopt;
  }

  std::optional<bool> HTMLMetaElement::parseBooleanValue(const std::string &value) const
  {
    std::string lower_value = value;
    std::transform(lower_value.begin(), lower_value.end(), lower_value.begin(), ::tolower);

    if (lower_value == "yes" || lower_value == "1" || lower_value == "true")
      return true;
    else if (lower_value == "no" || lower_value == "0" || lower_value == "false")
      return false;

    return std::nullopt;
  }

  std::string HTMLMetaElement::trim(const std::string &str) const
  {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
      return "";

    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
  }

  void HTMLMetaElement::onContentChanged()
  {
    if (isViewportMeta())
    {
      // Notify the document that viewport meta has changed
      auto doc = getOwnerDocument();
      if (doc)
      {
        doc->onViewportMetaChanged(std::dynamic_pointer_cast<HTMLMetaElement>(shared_from_this()));
      }
    }
  }

  void HTMLMetaElement::onNameChanged()
  {
    // Check if we became or stopped being a viewport meta
    auto doc = getOwnerDocument();
    if (doc)
    {
      doc->onViewportMetaChanged(std::dynamic_pointer_cast<HTMLMetaElement>(shared_from_this()));
    }
  }
}