// Copyright 2018 The Dawn & Tint Authors
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

#include <memory>
#include <string>
#include <utility>

#include <common/result.hpp>
#include <common/command_buffers/gpu/error_data.hpp>

namespace commandbuffers::gpu
{
  enum class InternalErrorType : uint32_t
  {
    kNone = 0,
    kValidation = 1,
    kDeviceLost = 2,
    kInternal = 4,
    kOutOfMemory = 8
  };

  // MaybeError and ResultOrError are meant to be used as return value for function that are not
  // expected to, but might fail. The handling of error is potentially much slower than successes.
  using MaybeError = jsar::Result<void, ErrorData>;

  template <typename T>
  using ResultOrError = jsar::Result<T, ErrorData>;

  namespace detail
  {
    template <typename T>
    struct UnwrapResultOrError
    {
      using type = T;
    };

    template <typename T>
    struct UnwrapResultOrError<ResultOrError<T>>
    {
      using type = T;
    };

    template <typename T>
    struct IsResultOrError
    {
      static constexpr bool value = false;
    };

    template <typename T>
    struct IsResultOrError<ResultOrError<T>>
    {
      static constexpr bool value = true;
    };

  }
}
