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
 * Shared reference is a template class that holds the shared pointer of a type.
 * 
 * You can use this class to hold the shared pointer and pass this class via C/C++ native pointer, reference and also the smart pointers.
 * This class is useful when you have to pass a C++ native pointer like v8::External or N-API External, it will help you to manage the
 * reference count in the whole life cycle of the object.
 */
template <typename T>
class SharedReference
{
public:
  SharedReference(std::shared_ptr<T> value) : value(value) {}

public:
  std::shared_ptr<T> value;
};

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
