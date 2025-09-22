#pragma once

#include <cassert>
#include <node/v8.h>
#include "./v8_object_wrap_base.hpp"

namespace scripting_base
{
  /**
   * JavaScript Object Holder is a template base class that holds the weak pointer to JavaScript object, this class is
   * useful to connect the reference object to a JavaScript object.
   *
   * @tparam T The type of the object wrap.
   */
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
     * @returns `true` if this holds a JavaScript object value, `false` otherwise.
     */
    inline bool hasJSObject() const
    {
      return value_ != nullptr;
    }
    /**
     * @returns The JavaScript object value.
     */
    inline ObjectWrapBase &getJSObject()
    {
      assert(hasJSObject());
      return *value_;
    }
    /**
     * @tparam T The type of the object wrap.
     * @returns The JavaScript object wrap as the specified type.
     */
    template <typename T>
      requires std::is_base_of_v<ObjectWrapBase, T>
    inline T &getJSObjectAs()
    {
      assert(hasJSObject());
      T *obj = static_cast<T *>(value_);
      assert(obj != nullptr && "The JS object is not of the expected type.");
      return *obj;
    }
    /**
     * @returns The instance reference.
     */
    inline ObjectWrapBase *getReference()
    {
      return value_;
    }
    /**
     * Set the weak reference to the specified value.
     *
     * @param instance The new value to set, or `nullptr` to set the reference value.
     */
    inline void setReference(ObjectWrapBase *value = nullptr)
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
    inline void ref(ObjectWrapBase *value)
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
    ObjectWrapBase *value_;
  };
}
