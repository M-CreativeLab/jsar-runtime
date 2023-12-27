#pragma once

#include <cstddef>
#include <stdint.h>

class VertexBuffer
{
public:
  float *data;
  size_t length;
  size_t byteLength;
};
