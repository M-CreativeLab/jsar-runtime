#pragma once

#include <cstdint>
#include <iostream>

class TrIdGenerator
{
public:
  TrIdGenerator(uint32_t initialId = 0) : counter(initialId) {}
  ~TrIdGenerator() {}

public:
  uint32_t get()
  {
    if (counter >= maxId)
      counter = 0;
    return counter++;
  }

private:
  uint32_t counter;
  uint32_t maxId = UINT32_MAX;
};
