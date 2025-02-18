#pragma once

namespace holocron
{
/** DEFINES */
#define CBINDGEN_IS_SERVO
#if defined(_WIN32) || defined(_WIN64)
#if defined(_WIN64)
#define SERVO_64_BITS
#else
#define SERVO_32_BITS
#endif
#elif defined(__GNUC__) || defined(__clang__)
#if defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
#define SERVO_64_BITS
#else
#define SERVO_32_BITS
#endif
#else
#error "Unsupported platform or compiler"
#endif
}
