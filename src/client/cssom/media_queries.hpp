#pragma once

#include <glm/glm.hpp>

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
     * Update viewport width only
     */
    void setViewportWidth(float width)
    {
      viewport_size_.x = width;
    }

    /**
     * Update viewport height only
     */
    void setViewportHeight(float height)
    {
      viewport_size_.y = height;
    }

  private:
    MediaType media_type_ = MediaType::Screen();
    glm::vec3 viewport_size_ = {ScreenWidth, ScreenHeight, VolumeDepth};
    float device_pixel_ratio_ = DevicePixelRatio;

    float root_font_size_ = 16.0f;
    float root_line_height_ = 1.0f;
  };
}
