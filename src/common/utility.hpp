#pragma once

#include <algorithm>
#include <string>
#include <unistd.h>

#ifndef TR_UNLIKELY
#if defined(__GNUC__) || defined(__clang__)
#define TR_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define TR_UNLIKELY(x) (x)
#endif
#endif

#ifndef TR_LIKELY
#if defined(__GNUC__) || defined(__clang__)
#define TR_LIKELY(x) __builtin_expect(!!(x), 1)
#else
#define TR_LIKELY(x) (x)
#endif
#endif

/**
 * Create a new string with all characters in lowercase.
 * 
 * @param str The source string to convert to lowercase.
 * @returns The new string with all characters in lowercase.
 */
inline std::string ToLowerCase(const std::string &str)
{
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(), ::tolower);
  return result;
}

/**
 * Create a new string with all characters in uppercase.
 * 
 * @param str The source string to convert to uppercase.
 * @returns The new string with all characters in uppercase.
 */
inline std::string ToUpperCase(const std::string &str)
{
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(), ::toupper);
  return result;
}
