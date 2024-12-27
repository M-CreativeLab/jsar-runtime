#pragma once

#include <cstdint>
#include <iostream>

template <typename IdType>
class TrIdTraits
{
public:
  static const IdType MaxId = UINT32_MAX;
};
template <>
class TrIdTraits<uint64_t>
{
public:
  static const uint64_t MaxId = UINT64_MAX;
};
template <>
class TrIdTraits<uint32_t>
{
public:
  static const uint32_t MaxId = UINT32_MAX;
};
template <>
class TrIdTraits<uint16_t>
{
public:
  static const uint16_t MaxId = UINT16_MAX;
};
template <>
class TrIdTraits<uint8_t>
{
public:
  static const uint8_t MaxId = UINT8_MAX;
};

/**
 * The simple Id generator for generating the id at JSAR runtime.
 *
 * This class implements the ID generating based on the simple self-incremental counter, it will not guarantee the accurate unique
 * ID, but it's enough for the runtime to generate the client ids such as the WebXR session, framebuffer id, etc.
 *
 * @tparam IdType The type of the id, it must be an integer type, and the default value is `uint32_t`.
 */
template <typename IdType = uint32_t>
  requires std::is_integral_v<IdType>
class TrIdGeneratorBase final
{
public:
  /**
   * Create a new Id generator.
   *
   * @param initialId The initial id to start with, and the default value is 0.
   * @param maxId The maximum id value, and the default value is `UINT32_MAX`.
   */
  TrIdGeneratorBase(IdType initialId = 0, IdType maxId = TrIdTraits<IdType>::MaxId)
      : counter(initialId),
        initialId(initialId),
        maxId(maxId)
  {
    if (maxId < initialId)
      throw std::runtime_error("The maximum id value should be greater than the initial id value.");
  }
  ~TrIdGeneratorBase() = default;

public:
  /**
   * @returns a `IdType` value that can be used as the id value, when the next possible id is reached to the maximum value, it
   *          will be reset to the initial id.
   */
  IdType get()
  {
    if (counter >= maxId)
      counter = initialId;
    return counter++;
  }
  /**
   * @returns a `IdType` value that represents the minimum id value, namely the `initialId`.
   */
  IdType min() const { return initialId; }
  /**
   * @returns a `uint32_t` value that represents the maximum id value, namely the `maxId`.
   */
  IdType max() const { return maxId; }

private:
  IdType counter;
  IdType initialId;
  IdType maxId;
};

using TrIdGenerator = TrIdGeneratorBase<uint32_t>;
