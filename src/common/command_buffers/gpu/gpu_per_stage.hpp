// Copyright 2017 The Dawn & Tint Authors
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <cassert>
#include <array>
#include <bitset>
#include <common/command_buffers/gpu/gpu_constants.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>

namespace commandbuffers
{
  enum class GPUSingleShaderStage
  {
    kVertex,
    kFragment,
    kCompute
  };

  static_assert(static_cast<uint32_t>(GPUSingleShaderStage::kVertex) < gpu_constants::kNumStages);
  static_assert(static_cast<uint32_t>(GPUSingleShaderStage::kFragment) < gpu_constants::kNumStages);
  static_assert(static_cast<uint32_t>(GPUSingleShaderStage::kCompute) < gpu_constants::kNumStages);

  static_assert(static_cast<uint32_t>(GPUShaderStage::kVertex) ==
                (1 << static_cast<uint32_t>(GPUSingleShaderStage::kVertex)));
  static_assert(static_cast<uint32_t>(GPUShaderStage::kFragment) ==
                (1 << static_cast<uint32_t>(GPUSingleShaderStage::kFragment)));
  static_assert(static_cast<uint32_t>(GPUShaderStage::kCompute) ==
                (1 << static_cast<uint32_t>(GPUSingleShaderStage::kCompute)));

  class GPUStages
  {
  public:
    static std::bitset<gpu_constants::kNumStages> IterateStages(GPUShaderStage stages);
    static GPUShaderStage StageBit(GPUSingleShaderStage stage);

    static constexpr GPUShaderStage kAllStages = static_cast<GPUShaderStage>((1 << gpu_constants::kNumStages) - 1);
  };

  template <typename T>
  class GPUPerStage
  {
  public:
    GPUPerStage() = default;
    explicit GPUPerStage(const T &initialValue)
    {
      data_.fill(initialValue);
    }

    T &operator[](GPUSingleShaderStage stage)
    {
      assert(static_cast<uint32_t>(stage) < gpu_constants::kNumStages);
      return data_[static_cast<uint32_t>(stage)];
    }

    const T &operator[](GPUSingleShaderStage stage) const
    {
      assert(static_cast<uint32_t>(stage) < gpu_constants::kNumStages);
      return data_[static_cast<uint32_t>(stage)];
    }

    T &operator[](GPUShaderStage stageBit)
    {
      uint32_t bit = static_cast<uint32_t>(stageBit);
      assert(bit != 0 && IsPowerOfTwo(bit) && bit <= (1 << gpu_constants::kNumStages));
      return data_[Log2(bit)];
    }
    const T &operator[](GPUShaderStage stageBit) const
    {
      uint32_t bit = static_cast<uint32_t>(stageBit);
      assert(bit != 0 && IsPowerOfTwo(bit) && bit <= (1 << gpu_constants::kNumStages));
      return data_[Log2(bit)];
    }

  private:
    static bool IsPowerOfTwo(uint64_t n)
    {
      assert(n != 0);
      return (n & (n - 1)) == 0;
    }
    static uint32_t Log2(uint32_t value)
    {
      assert(value != 0);
      return 31 - std::countl_zero(value);
    }

  private:
    std::array<T, gpu_constants::kNumStages> data_;
  };
}
