#pragma once

typedef double scalar;

class Vector3
{
public:
  scalar x = 0, y = 0, z = 0;
  Vector3();
  Vector3(scalar x, scalar y, scalar z);
};

class Quaternion
{
public:
  float x;
  float y;
  float z;
  float w;

public:
  Quaternion(float x, float y, float z, float w);
  Vector3 euler() const;
};
