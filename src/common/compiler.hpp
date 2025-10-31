#pragma once

// Copyright 2024 The Dawn & Tint Authors
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

// TR_COMPILER_IS(CLANG|GCC|MSVC): Compiler detection
//
// Note: clang masquerades as GCC on POSIX and as MSVC on Windows. It must be checked first.
#if defined(__clang__)
#define TR_COMPILER_IS_CLANG 1
#define TR_COMPILER_IS_GCC 0
#define TR_COMPILER_IS_MSVC 0
#elif defined(__GNUC__)
#define TR_COMPILER_IS_CLANG 0
#define TR_COMPILER_IS_GCC 1
#define TR_COMPILER_IS_MSVC 0
#elif defined(_MSC_VER)
#define TR_COMPILER_IS_CLANG 0
#define TR_COMPILER_IS_GCC 0
#define TR_COMPILER_IS_MSVC 1
#else
#error "Unsupported compiler"
#endif

// Use #if TR_COMPILER_IS(XXX) for compiler specific code.
// Do not use #ifdef or the naked macro TR_COMPILER_IS_XXX.
// This can help avoid common mistakes like not including "compiler.h" and falling into unwanted
// code block as usage of undefined macro "function" will be blocked by the compiler.
#define TR_COMPILER_IS(X) (1 == TR_COMPILER_IS_##X)

// TR_HAS_ATTRIBUTE
//
// A wrapper around `__has_attribute`. This test whether its operand is recognized by the compiler.
#if defined(__has_attribute)
#define TR_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#define TR_HAS_ATTRIBUTE(x) 0
#endif

// TR_HAS_CPP_ATTRIBUTE
//
// A wrapper around `__has_cpp_attribute`. This test whether its operand is recognized by the
// compiler.
#if defined(__has_cpp_attribute)
#define TR_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define TR_HAS_CPP_ATTRIBUTE(x) 0
#endif

// TR_BUILTIN_UNREACHABLE()
//
// Hints the compiler that a code path is unreachable.
#if TR_COMPILER_IS(MSVC)
#define TR_BUILTIN_UNREACHABLE() __assume(false)
#else
#define TR_BUILTIN_UNREACHABLE() __builtin_unreachable()
#endif

// TR_LIKELY(EXPR)
//
// Where available, hints the compiler that the expression will be true to help it generate code
// that leads to better branch prediction.
#if TR_COMPILER_IS(GCC) || TR_COMPILER_IS(CLANG)
#define TR_LIKELY(x) __builtin_expect(!!(x), 1)
#else
#define TR_LIKELY(x) (x)
#endif

// TR_UNLIKELY(EXPR)
//
// Where available, hints the compiler that the expression will be false to help it generate code
// that leads to better branch prediction.
#if TR_COMPILER_IS(GCC) || TR_COMPILER_IS(CLANG)
#define TR_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define TR_UNLIKELY(x) (x)
#endif

// TR_ASAN_ENABLED()
//
// Checks whether ASan is enabled.
#if TR_COMPILER_IS(CLANG)
#define TR_ASAN_ENABLED() __has_feature(address_sanitizer)
#elif TR_COMPILER_IS(GCC) || TR_COMPILER_IS(MSVC)
#if defined(__SANITIZE_ADDRESS__)
#define TR_ASAN_ENABLED() 1
#else
#define TR_ASAN_ENABLED() 0
#endif
#endif

// TR_NO_SANITIZE(instrumentation)
//
// Annotate a function or a global variable declaration to specify that a particular instrumentation
// or set of instrumentations should not be applied.
#if TR_HAS_ATTRIBUTE(no_sanitize)
#define TR_NO_SANITIZE(instrumentation) __attribute__((no_sanitize(instrumentation)))
#else
#define TR_NO_SANITIZE(instrumentation)
#endif
