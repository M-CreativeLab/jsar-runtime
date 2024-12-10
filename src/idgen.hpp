#pragma once

#include <cstdint>
#include <iostream>

/**
 * @brief The ID generator class in Transmute library.
 */
class TrIdGenerator
{
public:
  /**
   * Construct a new TrIdGenerator object
   * 
   * @param initialId The initial ID value.
   */
  TrIdGenerator(uint32_t initialId = 0) : counter(initialId), initialId(initialId) {}
  ~TrIdGenerator() {}

public:
  /**
   * Get the next ID.
   * @returns The next ID.
   */
  uint32_t get()
  {
    if (counter >= maxId)
      counter = initialId;
    return counter++;
  }
  /**
   * Get the minimum ID.
   */
  uint32_t min() const { return initialId; }

private:
  uint32_t counter;
  uint32_t initialId = 0;
  uint32_t maxId = UINT32_MAX;
};
