#pragma once

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

class TrViewport
{
public:
  // Default constructor
  TrViewport() noexcept
      : width_(0), height_(0), x_(0), y_(0) {}

  // Parameterized constructor
  TrViewport(uint32_t width, uint32_t height, uint32_t x = 0, uint32_t y = 0) noexcept
      : width_(width), height_(height), x_(x), y_(y) {}

public:
  // Checks if the viewport is equal to the specified values
  [[nodiscard]] bool isEqual(uint32_t width, uint32_t height, uint32_t x, uint32_t y) const noexcept
  {
    return width_ == width &&
           height_ == height &&
           x_ == x &&
           y_ == y;
  }

  // Sets the viewport values
  void set(uint32_t width, uint32_t height, uint32_t x, uint32_t y) noexcept
  {
    width_ = width;
    height_ = height;
    x_ = x;
    y_ = y;
  }

  // Converts the viewport to a string representation
  [[nodiscard]] std::string toString() const
  {
    std::stringstream ss;
    ss << "Viewport("
       << width_ << ", "
       << height_ << ", "
       << x_ << ", "
       << y_ << ")";
    return ss.str();
  }

  // Overloads the << operator for easy output
  friend std::ostream &operator<<(std::ostream &os, const TrViewport &viewport)
  {
    os << viewport.toString();
    return os;
  }

public:
  // Getter and Setter for width
  [[nodiscard]] uint32_t width() const noexcept { return width_; }
  void setWidth(uint32_t width) noexcept { width_ = width; }

  // Getter and Setter for height
  [[nodiscard]] uint32_t height() const noexcept { return height_; }
  void setHeight(uint32_t height) noexcept { height_ = height; }

  // Getter and Setter for x-coordinate
  [[nodiscard]] uint32_t x() const noexcept { return x_; }
  void setX(uint32_t x) noexcept { x_ = x; }

  // Getter and Setter for y-coordinate
  [[nodiscard]] uint32_t y() const noexcept { return y_; }
  void setY(uint32_t y) noexcept { y_ = y; }

private:
  uint32_t width_;  // Width of the viewport
  uint32_t height_; // Height of the viewport
  uint32_t x_;      // X-coordinate of the viewport
  uint32_t y_;      // Y-coordinate of the viewport
};
