#pragma once

#include <cmath>

/**
 * M_PI is not defined in Windows(MSVC), so we define it here.
 */
#ifndef M_PI
#define M_PI std::acos(-1)
#endif

typedef double scalar;

class Vector3
{
public:
  Vector3() : x(0), y(0), z(0)
  {
  }
  Vector3(scalar x, scalar y, scalar z) : x(x), y(y), z(z)
  {
  }

public:
  scalar x = 0, y = 0, z = 0;
};

class Quaternion
{
public:
  Quaternion() : x(0), y(0), z(0), w(1)
  {
  }
  Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
  {
  }
  Vector3 euler() const
  {
    Vector3 angle;
    const float zAxisY = y * z - x * w;
    const float limit = 0.4999999f;

    if (zAxisY < -limit)
    {
      angle.y = 2 * std::atan2(y, w);
      angle.x = M_PI / 2;
      angle.z = 0;
    }
    else if (zAxisY > limit)
    {
      angle.y = 2 * std::atan2(y, w);
      angle.x = -M_PI / 2;
      angle.z = 0;
    }
    else
    {
      const float sqw = w * w;
      const float sqz = z * z;
      const float sqx = x * x;
      const float sqy = y * y;
      angle.z = std::atan2(2.0 * (x * y + z * w), -sqz - sqx + sqy + sqw);
      angle.x = std::asin(-2.0 * zAxisY);
      angle.y = std::atan2(2.0 * (z * x + y * w), sqz - sqx - sqy + sqw);
    }
    return angle;
  }

public:
  float x;
  float y;
  float z;
  float w;
};
