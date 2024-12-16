#pragma once

#include <cstdint>
#include <iostream>

/**
 * The simple ID generator for generating the id at JSAR runtime.
 *
 * This class implements the ID generating based on the simple self-incremental counter, it will not guarantee the accurate unique
 * ID, but it's enough for the runtime to generate the client ids such as the WebXR session, framebuffer id, etc.
 */
class TrIdGenerator
{
public:
  /**
   * Create a new Id generator.
   *
   * @param initialId The initial id to start with, and the default value is 0.
   */
  TrIdGenerator(uint32_t initialId = 0) : counter(initialId), initialId(initialId) {}
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

private:
  uint32_t counter;
  uint32_t initialId = 0;
  uint32_t maxId = UINT32_MAX;
};
