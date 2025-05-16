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
    const MediaType& mediaType() const { return media_type_; }
    const glm::vec3& viewportSize() const { return viewport_size_; }
    const glm::vec3& devicePixelRatio() const { return device_pixel_ratio_; }

    float rootFontSize() const { return root_font_size_; }
    float rootLineHeight() const { return root_line_height_; }

  private:
    MediaType media_type_ = MediaType::Screen();
    glm::vec3 viewport_size_ = {ScreenWidth, ScreenHeight, VolumeDepth};
    glm::vec3 device_pixel_ratio_ = {DevicePixelRatio, DevicePixelRatio, DevicePixelRatio};

    float root_font_size_ = 16.0f;
    float root_line_height_ = 1.0f;
  };
}
