#pragma once

#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <type_traits>

template <typename IdType>
class TrIdTraits
{
public:
  static constexpr IdType MaxId = std::numeric_limits<IdType>::max();
};

template <>
class TrIdTraits<uint64_t>
{
public:
  static constexpr uint64_t MaxId = std::numeric_limits<uint64_t>::max();
};

template <>
class TrIdTraits<uint32_t>
{
public:
  static constexpr uint32_t MaxId = std::numeric_limits<uint32_t>::max();
};

template <>
class TrIdTraits<uint16_t>
{
public:
  static constexpr uint16_t MaxId = std::numeric_limits<uint16_t>::max();
};

template <>
class TrIdTraits<uint8_t>
{
public:
  static constexpr uint8_t MaxId = std::numeric_limits<uint8_t>::max();
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
   * @param maxId The maximum id value, and the default value is `TrIdTraits<IdType>::MaxId`.
   */
  TrIdGeneratorBase(IdType initialId = 0, IdType maxId = TrIdTraits<IdType>::MaxId)
      : counter_(initialId),
        initialId_(initialId),
        maxId_(maxId)
  {
    if (maxId_ < initialId_)
      throw std::runtime_error("The maximum id value should be greater than the initial id value.");
  }
  ~TrIdGeneratorBase() = default;

public:
  /**
   * @returns a `IdType` value that can be used as the id value, when the next possible id is reached to the maximum value, it
   *          will be reset to the initial id.
   */
  [[nodiscard]] IdType get() noexcept
  {
    if (counter_ >= maxId_)
      counter_ = initialId_;
    return counter_++;
  }

  /**
   * @returns a `IdType` value that represents the minimum id value, namely the `initialId`.
   */
  [[nodiscard]] IdType min() const noexcept { return initialId_; }

  /**
   * @returns a `IdType` value that represents the maximum id value, namely the `maxId`.
   */
  [[nodiscard]] IdType max() const noexcept { return maxId_; }

private:
  IdType counter_;   // Current counter value
  IdType initialId_; // Initial ID value
  IdType maxId_;     // Maximum ID value
};

using TrIdGenerator = TrIdGeneratorBase<uint32_t>;
