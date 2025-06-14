#pragma once

#include <algorithm>
#include <string>
#include <concepts>
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

#define TR_DISALLOW_NEW()                    \
public:                                      \
  void *operator new(size_t, void *location) \
  {                                          \
    return location;                         \
  }                                          \
                                             \
private:                                     \
  void *operator new(size_t) = delete

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
  SharedReference(std::shared_ptr<T> value)
      : value(value)
  {
  }

public:
  std::shared_ptr<T> value;
};

/**
 * JavaScript Object Holder is a template base class that holds the weak pointer to JavaScript object, this class is useful to connect
 * the reference object to a JavaScript object.
 *
 * @tparam T The type of the object that the weak reference holds.
 */
template <typename T>
class JSObjectHolder
{
public:
  JSObjectHolder()
      : value_(nullptr)
  {
  }
  JSObjectHolder(JSObjectHolder &that)
      : value_(that.value_)
  {
  }

public:
  /**
   * @returns `true` if this holder has a JavaScript object value, `false` otherwise.
   * @deprecated Use `hasJSObject` instead.
   */
  inline bool isJSObject() const
  {
    return value_ != nullptr;
  }
  /**
   * @returns `true` if this holds a JavaScript object value, `false` otherwise.
   */
  inline bool hasJSObject() const
  {
    return isJSObject();
  }
  /**
   * @returns The JavaScript object value.
   */
  inline T &getJSObject()
  {
    assert(isJSObject());
    return *value_;
  }
  /**
   * @returns The instance reference.
   */
  inline T *getReference()
  {
    return value_;
  }
  /**
   * Set the weak reference to the specified value.
   *
   * @param instance The new value to set, or `nullptr` to set the reference value.
   */
  inline void setReference(T *value = nullptr)
  {
    if (value != nullptr && value_ != nullptr)
      throw std::runtime_error("JavaScript holder's value must be reset a nullptr before setting a new instance.");
    value_ = value;
  }

  /**
   * Reference this object to the specified value, you must ensure that this object is not referenced to another object before calling this
   * method.
   *
   * @param value The value to reference.
   */
  inline void ref(T *value)
  {
    setReference(value);
  }

  /**
   * Unreference this object.
   */
  inline void unref()
  {
    setReference(nullptr);
  }

private:
  T *value_;
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

/**
 * Create a new string with the first character in uppercase and the rest in lowercase.
 *
 * @param str The source string to convert to capitalize.
 * @returns The new string with the first character in uppercase and the rest in lowercase.
 */
inline std::string ToCapitalize(std::string str)
{
  if (str.empty())
    return str;

  bool newWord = true;
  for (char &c : str)
  {
    if (newWord && std::isalpha(c))
    {
      c = std::toupper(c);
      newWord = false;
    }
    else if (std::isspace(c))
    {
      newWord = true;
    }
    else
    {
      c = std::tolower(c);
    }
  }
  return str;
}

namespace transmute::common
{
#ifdef ANDROID
  // Android NDK does not support the `std::derived_from` concept.
  // We have to define the `derived_from` concept manually.
  template <class Derived, class Base>
  concept derived_from = std::is_base_of_v<Base, Derived> &&
                         std::is_convertible_v<const volatile Derived *, const volatile Base *>;
#else
  template <class Derived, class Base>
  concept derived_from = std::derived_from<Derived, Base>;
#endif
}
