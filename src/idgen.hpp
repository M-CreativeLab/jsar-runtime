#pragma once

#include <cstdint>
#include <iostream>

class TrIdGenerator
{
public:
  TrIdGenerator(uint32_t initialId = 0) : counter(initialId), initialId(initialId) {}
  ~TrIdGenerator() {}

public:
  uint32_t get()
  {
    if (counter >= maxId)
      counter = initialId;
    return counter++;
  }

private:
  uint32_t counter;
  uint32_t initialId = 0;
  uint32_t maxId = UINT32_MAX;
};
