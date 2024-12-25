#pragma once

#include <cstdint>
#include <iostream>

/**
 * The simple ID generator for generating the id at JSAR runtime.
 *
 * This class implements the ID generating based on the simple self-incremental counter, it will not guarantee the accurate unique
 * ID, but it's enough for the runtime to generate the client ids such as the WebXR session, framebuffer id, etc.
 */
class TrIdGenerator final
{
public:
  /**
   * Create a new Id generator.
   *
   * @param initialId The initial id to start with, and the default value is 0.
   * @param maxId The maximum id value, and the default value is `UINT32_MAX`.
   */
  TrIdGenerator(uint32_t initialId = 0, uint32_t maxId = UINT32_MAX)
      : counter(initialId),
        initialId(initialId),
        maxId(maxId)
  {
    if (maxId < initialId)
      throw std::runtime_error("The maximum id value should be greater than the initial id value.");
  }
  ~TrIdGenerator() = default;

public:
  /**
   * @returns a `uint32_t` value that can be used as the id value, when the next possible id is reached to the maximum value, it
   *          will be reset to the initial id.
   */
  uint32_t get()
  {
    if (counter >= maxId)
      counter = initialId;
    return counter++;
  }
  /**
   * @returns a `uint32_t` value that represents the minimum id value, namely the `initialId`.
   */
  uint32_t min() const { return initialId; }
  /**
   * @returns a `uint32_t` value that represents the maximum id value, namely the `maxId`.
   */
  uint32_t max() const { return maxId; }

private:
  uint32_t counter;
  uint32_t initialId;
  uint32_t maxId;
};
