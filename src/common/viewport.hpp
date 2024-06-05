#pragma once

#include <cstdint>

class TrViewport
{
public:
  TrViewport() : width(0), height(0), x(0), y(0)
  {
  }
  TrViewport(uint32_t width, uint32_t height, uint32_t x = 0, uint32_t y = 0) : width(width), height(height), x(x), y(y)
  {
  }

public:
  inline bool isEqual(uint32_t width, uint32_t height, uint32_t x, uint32_t y)
  {
    return this->width == width &&
           this->height == height &&
           this->x == x &&
           this->y == y;
  }

public:
  uint32_t width;
  uint32_t height;
  uint32_t x;
  uint32_t y;
};
