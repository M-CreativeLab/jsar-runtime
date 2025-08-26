#pragma once

#include <glm/glm.hpp>
#include <algorithm>

#include "./media.hpp"
#include "./units.hpp"

namespace client_cssom
{
  /**
   * The CSS device context.
   */
  class Device
  {
  public:
    Device() = default;

    // Static device dimensions for device-width/device-height support
    static constexpr float DeviceWidth = ScreenWidth;
    static constexpr float DeviceHeight = ScreenHeight;
    static constexpr float DeviceDepth = VolumeDepth;

  public:
    const MediaType &mediaType() const
    {
      return media_type_;
    }
    const glm::vec3 &viewportSize() const
    {
      return viewport_size_;
    }
    const float devicePixelRatio() const
    {
      return device_pixel_ratio_;
    }
    float &devicePixelRatio()
    {
      return device_pixel_ratio_;
    }

    float rootFontSize() const
    {
      return root_font_size_;
    }
    float rootLineHeight() const
    {
      return root_line_height_;
    }

    // Viewport scaling properties
    float initialScale() const
    {
      return initial_scale_;
    }
    float minimumScale() const
    {
      return minimum_scale_;
    }
    float maximumScale() const
    {
      return maximum_scale_;
    }
    bool userScalable() const
    {
      return user_scalable_;
    }

    void setInitialScale(float scale)
    {
      initial_scale_ = std::clamp(scale, 0.1f, 10.0f);
      // Apply initial scale to device pixel ratio
      device_pixel_ratio_ = initial_scale_;
    }
    void setMinimumScale(float scale)
    {
      minimum_scale_ = std::clamp(scale, 0.1f, 10.0f);
    }
    void setMaximumScale(float scale)
    {
      maximum_scale_ = std::clamp(scale, 0.1f, 10.0f);
    }
    void setUserScalable(bool scalable)
    {
      user_scalable_ = scalable;
    }

    /**
     * Update viewport size from viewport meta tag
     */
    void setViewportSize(float width, float height)
    {
      viewport_size_.x = width;
      viewport_size_.y = height;
      // Keep depth unchanged
    }

    /**
     * Update viewport width only, with device-width support
     */
    void setViewportWidth(float width, bool use_device_width = false)
    {
      if (use_device_width)
      {
        viewport_size_.x = DeviceWidth;
      }
      else
      {
        viewport_size_.x = width;
      }
    }

    /**
     * Update viewport height only, with device-height support
     */
    void setViewportHeight(float height, bool use_device_height = false)
    {
      if (use_device_height)
      {
        viewport_size_.y = DeviceHeight;
      }
      else
      {
        viewport_size_.y = height;
      }
    }

  private:
    MediaType media_type_ = MediaType::Screen();
    glm::vec3 viewport_size_ = {ScreenWidth, ScreenHeight, VolumeDepth};
    float device_pixel_ratio_ = DevicePixelRatio;

    // Viewport scaling properties
    float initial_scale_ = 1.0f;
    float minimum_scale_ = 0.1f;
    float maximum_scale_ = 10.0f;
    bool user_scalable_ = true;

    float root_font_size_ = 16.0f;
    float root_line_height_ = 1.0f;
  };
}
