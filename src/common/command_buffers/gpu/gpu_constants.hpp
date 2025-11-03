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

#include <cstdint>

#define WGPU_ARRAY_LAYER_COUNT_UNDEFINED (UINT32_MAX)
#define WGPU_COPY_STRIDE_UNDEFINED (UINT32_MAX)
#define WGPU_DEPTH_CLEAR_VALUE_UNDEFINED (NAN)
#define WGPU_DEPTH_SLICE_UNDEFINED (UINT32_MAX)
#define WGPU_LIMIT_U32_UNDEFINED (UINT32_MAX)
#define WGPU_LIMIT_U64_UNDEFINED (UINT64_MAX)
#define WGPU_MIP_LEVEL_COUNT_UNDEFINED (UINT32_MAX)
#define WGPU_QUERY_SET_INDEX_UNDEFINED (UINT32_MAX)
#define WGPU_STRLEN (SIZE_MAX)
#define WGPU_WHOLE_MAP_SIZE (SIZE_MAX)
#define WGPU_WHOLE_SIZE (UINT64_MAX)

namespace commandbuffers::gpu_constants
{
  static constexpr uint32_t kMaxBindGroups = 4u;
  static constexpr uint32_t kMaxBindingsPerBindGroup = 1000u;
  static constexpr uint8_t kMaxVertexAttributes = 30u;
  static constexpr uint8_t kMaxVertexBuffers = 8u;
  static constexpr uint32_t kMaxVertexBufferArrayStride = 2048u;
  static constexpr uint32_t kMaxBindGroupsPlusVertexBuffers = 24u;
  static constexpr uint32_t kNumStages = 3;
  static constexpr uint8_t kMaxColorAttachments = 8u;
  static constexpr uint32_t kTextureBytesPerRowAlignment = 256u;
  static constexpr uint32_t kQueryResolveAlignment = 256u;
  static constexpr uint32_t kMaxInterStageShaderVariables = 16u;
  static constexpr uint64_t kAssumedMaxBufferSize = 0x80000000u; // Use 2 GB when the limit is unavailable

  static constexpr uint32_t kArrayLayerCountUndefined = WGPU_ARRAY_LAYER_COUNT_UNDEFINED;
  static constexpr uint32_t kCopyStrideUndefined = WGPU_COPY_STRIDE_UNDEFINED;
  static constexpr float kDepthClearValueUndefined = std::numeric_limits<float>::quiet_NaN();
  static constexpr uint32_t kDepthSliceUndefined = WGPU_DEPTH_SLICE_UNDEFINED;
  static constexpr uint32_t kLimitU32Undefined = WGPU_LIMIT_U32_UNDEFINED;
  static constexpr uint64_t kLimitU64Undefined = WGPU_LIMIT_U64_UNDEFINED;
  static constexpr uint32_t kMipLevelCountUndefined = WGPU_MIP_LEVEL_COUNT_UNDEFINED;
  static constexpr uint32_t kQuerySetIndexUndefined = WGPU_QUERY_SET_INDEX_UNDEFINED;
  static constexpr size_t kStrlen = WGPU_STRLEN;
  static constexpr size_t kWholeMapSize = WGPU_WHOLE_MAP_SIZE;
  static constexpr uint64_t kWholeSize = WGPU_WHOLE_SIZE;
};
