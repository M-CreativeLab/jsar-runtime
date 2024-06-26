#pragma once

#include <cstdint>
#include <iostream>
#include <string>

using namespace std;

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
  string toString() const
  {
    return "Viewport(" +
           to_string(width) + ", " +
           to_string(height) + ", " +
           to_string(x) + "," +
           to_string(y) +
           ")";
  }
  friend ostream &operator<<(ostream &os, const TrViewport& viewport)
  {
    os << viewport.toString();
    return os;
  }

public:
  uint32_t width;
  uint32_t height;
  uint32_t x;
  uint32_t y;
};
