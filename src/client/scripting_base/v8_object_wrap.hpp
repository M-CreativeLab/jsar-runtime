#pragma once

#include <memory>
#include <vector>
#include <assert.h>
#include <iostream>

#include <node/v8.h>
#include <node/node_api.h>
#include <common/utility.hpp>

#include "./v8_object_holder.hpp"
#include "./v8_object_wrap_base.hpp"

namespace scripting_base
{
  /**
   * A base class for wrapping C++ objects in v8::Object instances.
   *
   * This class is useful for wrapping C++ objects when the N-API's ObjectWrap is not available such as in the case of
   * using the V8 API directly.
   *
   * @tparam T The class to wrap.
   * @tparam D The type of the optional inner instance.
   * @tparam B The base class of T, if T is derived from another ObjectWrap class.
   */
  template <typename T, typename D = void, typename B = void>
  class ObjectWrap : public ObjectWrapBase
  {
    friend class ObjectWrap<T, D, B>;

  public:
    /**
     * The name of the `ObjectWrap` class.
     *
     * @returns The name of the class.
     */
    static std::string Name()
    {
      return "Object";
    }

    static v8::Local<v8::FunctionTemplate> GetFunctionTemplate(v8::Isolate *isolate)
    {
      return function_template_.Get(isolate);
    }

    static v8::Local<v8::Function> GetConstructorFunction(v8::Isolate *isolate)
    {
      return constructor_handle_.Get(isolate);
    }

    /**
     * Configure the function template, this method is called when the class is being initialized, the subclass can
     * override this method to configure the function template.
     *
     * @param isolate The v8::Isolate instance
     * @param tpl The v8::FunctionTemplate instance
     */
    static void ConfigureFunctionTemplate(v8::Isolate *, v8::Local<v8::FunctionTemplate>)
    {
      // Override this method in the derived class to configure the function template
    }

    /**
     * Create the instance of the class T and wrap it in a v8::Object
     *
     * @tparam Args The types of the arguments to pass to the constructor of T
     *
     * @param isolate The v8::Isolate instance
     * @param inner The optional inner instance of the class D
     * @returns The wrapped v8::Object
     */
    static v8::Local<v8::Object> NewInstance(napi_env napiEnv, std::shared_ptr<D> inner = nullptr)
    {
      if constexpr (!std::is_same_v<D, void>)
      {
        assert(inner != nullptr && "inner must not be null when D is not void");
      }

      v8::Isolate *isolate = v8::Isolate::GetCurrent();
      v8::EscapableHandleScope scope(isolate);

      v8::Local<v8::Object> jsThis = NewInstance(isolate, inner);
      T *instance = Unwrap(jsThis);
      instance->setNapiEnv(napiEnv);

      return scope.Escape(jsThis);
    }

    /**
     * Create the instance of the class T and wrap it in a v8::Object
     *
     * @param isolate The v8::Isolate instance
     * @param inner The optional inner instance of the class D
     * @returns The wrapped v8::Object
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<D> inner)
    {
      if constexpr (!std::is_same_v<D, void>)
      {
        assert(inner != nullptr && "inner must not be null when D is not void");
      }

      v8::EscapableHandleScope scope(isolate);
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::Local<v8::Function> constructor = constructor_handle_.Get(isolate);
      if (constructor.IsEmpty()) [[unlikely]]
      {
        std::cerr << "Constructor is not initialized for " << T::Name() << "()" << std::endl;
        return scope.Escape(v8::Local<v8::Object>());
      }

      std::vector<v8::Local<v8::Value>> args;
      v8::Local<v8::Object> jsThis = constructor->NewInstance(context, 0, nullptr).ToLocalChecked();
      if (jsThis.IsEmpty()) [[unlikely]]
      {
        std::cerr << "Failed to create new instance of " << T::Name() << "()" << std::endl;
        return scope.Escape(v8::Local<v8::Object>());
      }

      // Update the inner reference
      T *instance = Unwrap(jsThis);
      instance->setInner(inner);

      // Return the created instance
      return scope.Escape(jsThis);
    }

    /**
     * Get the instance object.
     */
    static v8::Local<v8::Object> GetOrNewInstance(v8::Isolate *isolate, std::shared_ptr<D> inner)
    {
      if constexpr (!std::is_base_of_v<JSObjectHolder, D>)
      {
        assert(false && "inner type must inherit from `JSObjectHolder`");
      }

      if (inner == nullptr) [[unlikely]]
      {
        return v8::Local<v8::Object>();
      }

      if (inner->hasJSObject())
      {
        v8::EscapableHandleScope scope(isolate);
        ObjectWrapBase *objectWrap = inner->getJSObjectWrap();
        return scope.Escape(objectWrap->getJSObject(isolate));
      }
      else
      {
        return NewInstance(isolate, inner);
      }
    }

    static void Wrap(v8::Isolate *isolate, v8::Local<v8::Object> object, T *instance)
    {
      assert(isolate != nullptr && "isolate must not be null");
      assert(!object.IsEmpty() && "object must not be null");
      assert(instance != nullptr && "instance must not be null");

      v8::HandleScope scope(isolate);
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      object->SetInternalField(0, v8::External::New(isolate, instance));
    }

    /**
     * Unwrap the v8::Object to get the instance of the class T
     *
     * @param object The v8::Object to unwrap
     * @returns The instance of the class T
     */
    static T *Unwrap(v8::Local<v8::Object> object)
    {
      auto externalValue = object->GetInternalField(0);
      if (externalValue.IsEmpty())
        return nullptr;
      return static_cast<T *>(v8::Local<v8::External>::Cast(externalValue)->Value());
    }

    /**
     * Initialize the class and return the constructor function.
     *
     * @param isolate The v8::Isolate instance
     */
    static v8::Local<v8::Function> Initialize(v8::Isolate *isolate)
    {
      assert(isolate != nullptr);

      // Return the existing constructor if already initialized
      if (ObjectWrap<T, D, B>::initialized_ == true)
      {
        return ObjectWrap<T, D, B>::GetConstructorFunction(isolate);
      }

      v8::HandleScope scope(isolate);
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, ObjectWrap<T, D, B>::Constructor);
      tpl->SetClassName(v8::String::NewFromUtf8(isolate, T::Name().c_str()).ToLocalChecked());
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // Set up inheritance if `B` is specified
      if constexpr (!std::is_same_v<B, void>)
      {
        // Ensure base class is initialized first
        B::Initialize(isolate);
        v8::Local<v8::FunctionTemplate> base = B::GetFunctionTemplate(isolate);
        if (!base.IsEmpty())
        {
          tpl->Inherit(base);
        }
      }

      T::ConfigureFunctionTemplate(isolate, tpl);

      // Update the persistent handles
      v8::Local<v8::Function> constructor = tpl->GetFunction(context).ToLocalChecked();
      ObjectWrap<T, D, B>::constructor_handle_.Reset(isolate, constructor);
      ObjectWrap<T, D, B>::function_template_.Reset(isolate, tpl);
      ObjectWrap<T, D, B>::initialized_ = true;
      return constructor;
    }

  public:
    ObjectWrap(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
        : ObjectWrapBase(isolate)
    {
    }

    virtual ~ObjectWrap()
    {
    }

  public:
    void setInner(std::shared_ptr<D> data)
    {
      inner_handle_ = data;
      onDataSet(data);

      // Set the weak reference back to this object if D is derived from `JSObjectHolder`.
      if constexpr (std::is_base_of_v<JSObjectHolder, D>)
      {
        if (inner_handle_ != nullptr)
          inner_handle_->setReference(this);
      }
    }

    v8::Local<v8::Value> value() const
    {
      return object_handle_.Get(current_isolate_);
    }
    std::shared_ptr<D> inner() const
    {
      return inner_handle_;
    }

  protected:
    virtual void onDataSet(std::shared_ptr<D> data)
    {
    }

  private:
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
      v8::Isolate *isolate = args.GetIsolate();

      if (!args.IsConstructCall())
      {
        isolate->ThrowException(v8::Exception::TypeError(
          v8::String::NewFromUtf8(isolate, "Illegal constructor").ToLocalChecked()));
        return;
      }

      T *instance = new T(isolate, args);
      assert(instance != nullptr && "Failed to create instance");

      auto jsObject = args.This();
      Wrap(isolate, jsObject, instance);

      instance->object_handle_.Reset(isolate, jsObject);
      instance->object_handle_.SetWeak(instance, Finalizer, v8::WeakCallbackType::kParameter);

      args.GetReturnValue().Set(jsObject);
    }

    static void Finalizer(const v8::WeakCallbackInfo<T> &data)
    {
      T *instance = data.GetParameter();
      if (instance != nullptr)
        instance->object_handle_.Reset();
      data.SetSecondPassCallback(Cleanup);
    }

    static void Cleanup(const v8::WeakCallbackInfo<T> &data)
    {
      T *instance = data.GetParameter();
      if (instance != nullptr)
        delete instance;
    }

  protected:
    // Inner is a weak pointer to the optional inner instance of the class D.
    std::shared_ptr<D> inner_handle_;

  private:
    static thread_local inline bool initialized_ = false;
    static thread_local inline v8::Persistent<v8::FunctionTemplate> function_template_;
    static thread_local inline v8::Persistent<v8::Function> constructor_handle_;
  };
}
