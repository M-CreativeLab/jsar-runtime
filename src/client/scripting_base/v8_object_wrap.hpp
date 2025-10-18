#pragma once

#include <thread>
#include <memory>
#include <vector>
#include <assert.h>
#include <iostream>

#include <node/v8.h>
#include <node/node_api.h>
#include <common/utility.hpp>

#include "./v8_object_holder.hpp"
#include "./v8_object_wrap_base.hpp"
#include "./v8_utils.hpp"

namespace scripting_base
{
  template <typename T, typename TCallback>
  struct FunctionCallbackData
  {
    TCallback callback;
    void *data;
  };

  template <typename T, typename TGetterCallback, typename TSetterCallback>
  struct AccessorCallbackData
  {
    TGetterCallback getterCallback;
    TSetterCallback setterCallback;
    void *data;
  };

  constexpr int CONSTRUCTING_CONTEXT_FLAG = 0xFE32;

  /**
   * A struct to represent the context to construct the object.
   */
  struct ConstructingContext
  {
    enum : uint8_t
    {
      kScript = 0xf,
      kNative,
    };

    int flag;
    uint8_t source = kScript;

    ConstructingContext(uint8_t source)
        : flag(CONSTRUCTING_CONTEXT_FLAG)
        , source(source)
    {
    }

    bool isValid() const
    {
      return flag == CONSTRUCTING_CONTEXT_FLAG;
    }
    bool isNativeCall() const
    {
      return isValid() && source == kNative;
    }
    bool isScriptCall() const
    {
      return isValid() && source == kScript;
    }
  };

  /**
   * A base class for wrapping C++ class in `v8::Object` instance.
   *
   * This class is useful for wrapping C++ objects when the N-API's ObjectWrap is not available such as in the case of
   * using the V8 API directly.
   *
   * @tparam T The class to wrap.
   * @tparam TData The type of the optional data handle.
   * @tparam TBase The base class of T, if T is derived from another ObjectWrap class.
   */
  template <typename T, typename TData = void, typename TBase = BaseObject>
  class ObjectWrap : public TBase
  {
    friend class ObjectWrap<T, TData, TBase>;

  protected:
    /**
     * Instance function are used to define the instance methods and property getters/setters functions.
     */
    using InstanceFunctionCallback = void (T::*)(const v8::FunctionCallbackInfo<v8::Value> &);
    using InstanceFunctionCallbackData = FunctionCallbackData<T, InstanceFunctionCallback>;

    /**
     * Instance accessor callbacks are used to define the instance's value getters/setters.
     */
    using InstanceGetterCallback = void (T::*)(const v8::PropertyCallbackInfo<v8::Value> &);
    using InstanceSetterCallback = void (T::*)(v8::Local<v8::Value>, const v8::PropertyCallbackInfo<void> &);
    using InstanceAccessorCallbackData = AccessorCallbackData<T, InstanceGetterCallback, InstanceSetterCallback>;

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

    /**
     * Checks if this class requires native constructor call.
     */
    static bool NativeConstructorRequired()
    {
      return false;
    }

    static v8::Local<v8::FunctionTemplate> GetFunctionTemplate(v8::Isolate *isolate)
    {
      v8::EscapableHandleScope scope(isolate);
      return scope.Escape(function_template_.Get(isolate));
    }

    static v8::Local<v8::Function> GetConstructorFunction(v8::Isolate *isolate)
    {
      v8::EscapableHandleScope scope(isolate);
      return scope.Escape(constructor_handle_.Get(isolate));
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
     * @brief Checks if a given V8 value is an instance of the class associated with this wrapper.
     *
     * This function verifies whether the provided V8 value is an object and whether it is an
     * instance of the constructor function associated with this class. It uses the V8 API to
     * perform the type check.
     *
     * @param isolate A pointer to the V8 isolate in which the check is performed.
     * @param value The V8 value to be checked.
     * @return true if the value is an object and an instance of the associated class, false otherwise.
     */
    static bool IsInstanceOf(v8::Isolate *isolate, v8::Local<v8::Value> value)
    {
      if (!value->IsObject())
        return false;

      v8::Local<v8::Object> obj = value.As<v8::Object>();
      v8::Local<v8::Function> constructor = T::GetConstructorFunction(isolate);
      if (constructor.IsEmpty())
        return false;
      return obj->InstanceOf(isolate->GetCurrentContext(), constructor).FromMaybe(false);
    }

    /**
     * Check if the constructor call is from native code, such as called from `NewInstance()`.
     * 
     * @param args The function callback info.
     * @return true if the constructor call is from native code, false otherwise.
     */
    static bool IsNativeConstructCall(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
      if (args.Length() < 1)
        return false;
      if (!args[0]->IsExternal())
        return false;

      v8::HandleScope scope(args.GetIsolate());
      v8::Local<v8::External> external = args[0].As<v8::External>();
      ConstructingContext *context = static_cast<ConstructingContext *>(external->Value());
      if (context == nullptr)
        return false;
      return context->isNativeCall();
    }

    /**
     * Create the instance of the class T and wrap it in a v8::Object
     *
     * @tparam Args The types of the arguments to pass to the constructor of T
     *
     * @param isolate The v8::Isolate instance
     * @param data The optional data instance of the class D
     * @returns The wrapped v8::Object
     */
    static v8::Local<v8::Object> NewInstance(napi_env napiEnv, std::shared_ptr<TData> handle = nullptr)
    {
      if constexpr (!std::is_same_v<TData, void>)
      {
        assert(handle != nullptr && "data handle must not be null when D is not void");
      }

      v8::Isolate *isolate = v8::Isolate::GetCurrent();
      v8::EscapableHandleScope scope(isolate);

      v8::Local<v8::Object> jsThis = NewInstance(isolate, handle);
      T *instance = T::Unwrap(isolate, jsThis);
      instance->setNapiEnv(napiEnv);
      return scope.Escape(jsThis);
    }

    /**
     * Create the instance of the class T and wrap it in a v8::Object
     *
     * @param isolate The v8::Isolate instance
     * @param handle The optional data instance of the class `TData`
     * @returns The wrapped `v8::Object`
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<TData> handle)
    {
      v8::EscapableHandleScope scope(isolate);
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::Local<v8::Function> constructor = constructor_handle_.Get(isolate);
      if (constructor.IsEmpty()) [[unlikely]]
      {
        std::cerr << "Constructor is not initialized for " << T::Name() << "()" << std::endl;
        return scope.Escape(v8::Local<v8::Object>());
      }

      std::vector<v8::Local<v8::Value>> args;
      if constexpr (!std::is_same_v<TData, void>)
      {
        // Only mark native constructing when the handle type is specified (not `void`)
        auto constructingContext = new ConstructingContext(ConstructingContext::kNative);
        v8::Local<v8::External> option = v8::External::New(isolate, constructingContext);
        args.push_back(option);
      }

      v8::TryCatch tryCatch(isolate);
      v8::Local<v8::Object> jsThis;
      v8::MaybeLocal<v8::Object> maybeResult = constructor->NewInstance(context, args.size(), args.data());
      if (maybeResult.IsEmpty() || tryCatch.HasCaught())
      {
        std::cerr << "Failed to create new instance of " << T::Name() << "(): "
                  << scripting_base::ReportExceptionToString(isolate, tryCatch.Exception()) << std::endl;
        return scope.Escape(v8::Local<v8::Object>());
      }
      else
      {
        jsThis = maybeResult.ToLocalChecked();
        if (jsThis.IsEmpty() || !jsThis->IsObject()) [[unlikely]]
        {
          std::cerr << "Failed to create new instance of " << T::Name() << "(): Empty this object" << std::endl;
          return scope.Escape(v8::Local<v8::Object>());
        }
      }

      // Unwrap and check if the instance is valid
      {
        T *instance = T::Unwrap(isolate, jsThis);
        assert(instance != nullptr && "Failed to unwrap the instance");

        // Set the data handle if `TData` is specified and handle is not null
        if constexpr (!std::is_same_v<TData, void>)
        {
          // Update the data reference
          instance->setData(handle);
        }

        // Call the onCreated hook
        instance->onCreated();
      }

      // Return the created instance
      return scope.Escape(jsThis);
    }

    /**
     * Get the instance object.
     */
    static v8::Local<v8::Object> GetOrNewInstance(v8::Isolate *isolate, std::shared_ptr<TData> handle)
    {
      if constexpr (!std::is_base_of_v<JSObjectHolder, TData>)
      {
        assert(false && "data type must inherit from `JSObjectHolder`");
      }

      assert(handle != nullptr && "handle must not be null");
      if (handle->hasJSObject())
      {
        v8::EscapableHandleScope scope(isolate);
        BaseObject *object = handle->getJSObjectWrap();
        assert(object != nullptr && "object wrap must not be null");
        v8::Local<v8::Object> value = object->getJSObject(isolate);
        if (value.IsEmpty())
        {
          std::cerr << "Failed to get JS object from the handle(" << T::Name() << "@" << object << ")" << std::endl;
          assert(false && "failed to get JS object from the handle");
        }
        return scope.Escape(value);
      }
      else
      {
        return T::NewInstance(isolate, handle);
      }
    }

    static void Wrap(v8::Isolate *isolate, v8::Local<v8::Object> object, T *instance)
    {
      assert(isolate != nullptr && "isolate must not be null");
      assert(!object.IsEmpty() && "object must not be null");
      assert(instance != nullptr && "instance must not be null");

      v8::HandleScope scope(isolate);
      object->SetInternalField(0, v8::External::New(isolate, instance));
      instance->Reset(object);
    }

    /**
     * Unwrap the v8::Object to get the instance of the class T
     *
     * @param object The v8::Object to unwrap
     * @returns The instance of the class T
     */
    static T *Unwrap(v8::Isolate *isolate, v8::Local<v8::Object> object)
    {
      v8::HandleScope scope(isolate);
      if (object.IsEmpty() ||
          !object->IsObject() ||
          object->InternalFieldCount() <= 0) [[unlikely]]
      {
        return nullptr;
      }

      auto externalValue = object->GetInternalField(0);
      if (externalValue.IsEmpty())
        return nullptr;
      return static_cast<T *>(externalValue.As<v8::External>()->Value());
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
      if (T::initialized_ == true)
      {
        return T::GetConstructorFunction(isolate);
      }

      v8::EscapableHandleScope scope(isolate);
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, T::Constructor);
      if (tpl.IsEmpty()) [[unlikely]]
      {
        std::cerr << "Failed to create function template for " << T::Name()
                  << std::endl;
        assert(false && "Failed to create function template");
      }
      tpl->SetClassName(v8::String::NewFromUtf8(isolate, T::Name().c_str()).ToLocalChecked());
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // Set up inheritance if `TBase` is specified
      if constexpr (!std::is_same_v<TBase, BaseObject>)
      {
        // Ensure base class is initialized first
        TBase::Initialize(isolate);
        v8::Local<v8::FunctionTemplate> baseType = TBase::GetFunctionTemplate(isolate);
        if (!baseType.IsEmpty())
        {
          tpl->Inherit(baseType);
        }
      }

      T::ConfigureFunctionTemplate(isolate, tpl);

      // Update the persistent handles
      v8::Local<v8::Function> constructor;
      if (tpl->GetFunction(context).ToLocal(&constructor) != true || constructor.IsEmpty()) [[unlikely]]
      {
        std::cerr << "Failed to get constructor `v8::Function` from function template for " << T::Name()
                  << std::endl;
        assert(false && "Failed to get constructor function");
      }

      T::constructor_handle_.Reset(isolate, constructor);
      T::function_template_.Reset(isolate, tpl);
      T::initialized_ = true;
      return scope.Escape(constructor);
    }

  protected:
    /// Convienience helpers to define values, constants, methods, properties and accessors.

    /**
     * Sets a read-only instance property with the specified name and value on a V8 ObjectTemplate.
     *
     * @param isolate         The V8 isolate in which to create the property.
     * @param objectTemplate  The V8 ObjectTemplate to which the property will be added.
     * @param name            The name of the property to set.
     * @param value           The value to assign to the property.
     *
     * This function adds a read-only property with the given name and value to the provided
     * ObjectTemplate, making it available on instances created from this template.
     */
    template <typename V>
    static void IntegerConstant(v8::Isolate *isolate,
                                v8::Local<v8::FunctionTemplate> tpl,
                                const char *name,
                                V value)
    {
      v8::HandleScope scope(isolate);
      v8::Local<v8::String> nameString = v8::String::NewFromUtf8(isolate, name).ToLocalChecked();
      v8::Local<v8::Number> valueNumber = v8::Integer::New(isolate, static_cast<int>(value));
      v8::PropertyAttribute attributes = static_cast<v8::PropertyAttribute>(v8::PropertyAttribute::ReadOnly |
                                                                            v8::PropertyAttribute::DontDelete);
      tpl->Set(nameString, valueNumber, attributes);
      tpl->PrototypeTemplate()->Set(nameString, valueNumber, attributes);
    }
    /**
     * Create a standardized method callback for instance methods.
     * 
     * @param isolate The v8::Isolate instance.
     * @param objectTemplate The object template to which the method will be added.
     * @param name The name of the method.
     * @param callback The method callback function.
     */
    static void InstanceMethod(v8::Isolate *isolate,
                               v8::Local<v8::ObjectTemplate> objectTemplate,
                               v8::Local<v8::Name> name,
                               InstanceFunctionCallback callback)
    {
      v8::HandleScope scope(isolate);

      auto callbackData = new InstanceFunctionCallbackData{callback, nullptr};
      v8::Local<v8::External> dataValue = v8::External::New(isolate, callbackData);

      // Manage the lifetime of the callback data
      auto handle = std::make_unique<v8::Persistent<v8::External>>(isolate, dataValue);
      auto releaseCallback = [](const v8::WeakCallbackInfo<InstanceFunctionCallbackData> &data)
      {
        InstanceFunctionCallbackData *callbackData = data.GetParameter();
        if (callbackData != nullptr)
          delete callbackData;
      };
      handle->SetWeak(callbackData, releaseCallback, v8::WeakCallbackType::kParameter);
      T::callback_data_handles_.emplace_back(std::move(handle));

      objectTemplate->Set(name, v8::FunctionTemplate::New(isolate, MethodWrapper, dataValue));
    }
    /**
     * Create a standardized method callback for instance methods with string name.
     */
    static void InstanceMethod(v8::Isolate *isolate,
                               v8::Local<v8::ObjectTemplate> objectTemplate,
                               const char *name,
                               InstanceFunctionCallback callback)
    {
      v8::HandleScope scope(isolate);
      T::InstanceMethod(isolate,
                        objectTemplate,
                        v8::String::NewFromUtf8(isolate, name).ToLocalChecked(),
                        callback);
    }
    /**
     * Create a standardized accessor callback for instance properties.
     */
    static void InstanceAccessor(v8::Isolate *isolate,
                                 v8::Local<v8::ObjectTemplate> objectTemplate,
                                 const char *name,
                                 InstanceGetterCallback getter,
                                 InstanceSetterCallback setter,
                                 v8::PropertyAttribute attributes = v8::PropertyAttribute::None)
    {
      auto callbackData = new InstanceAccessorCallbackData{getter, setter, nullptr};
      v8::Local<v8::External> dataValue = v8::External::New(isolate, callbackData);

      // Manage the lifetime of the callback data
      auto handle = std::make_unique<v8::Persistent<v8::External>>(isolate, dataValue);
      auto releaseCallback = [](const v8::WeakCallbackInfo<InstanceAccessorCallbackData> &data)
      {
        InstanceAccessorCallbackData *callbackData = data.GetParameter();
        if (callbackData != nullptr)
          delete callbackData;
      };
      handle->SetWeak(callbackData, releaseCallback, v8::WeakCallbackType::kParameter);
      T::callback_data_handles_.emplace_back(std::move(handle));

      // Add the accessor to the object template
      objectTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, name).ToLocalChecked(),
                                  GetterWrapper,
                                  SetterWrapper,
                                  dataValue,
                                  v8::AccessControl::DEFAULT,
                                  attributes);
    }
    static void InstanceReadonlyAccessor(v8::Isolate *isolate,
                                         v8::Local<v8::ObjectTemplate> objectTemplate,
                                         const char *name,
                                         InstanceGetterCallback getter,
                                         v8::PropertyAttribute attributes = v8::PropertyAttribute::ReadOnly)
    {
      auto callbackData = new InstanceAccessorCallbackData{getter, nullptr, nullptr};
      v8::Local<v8::External> dataValue = v8::External::New(isolate, callbackData);

      // Manage the lifetime of the callback data
      auto handle = std::make_unique<v8::Persistent<v8::External>>(isolate, dataValue);
      auto releaseCallback = [](const v8::WeakCallbackInfo<InstanceAccessorCallbackData> &data)
      {
        InstanceAccessorCallbackData *callbackData = data.GetParameter();
        if (callbackData != nullptr)
          delete callbackData;
      };
      handle->SetWeak(callbackData, releaseCallback, v8::WeakCallbackType::kParameter);
      T::callback_data_handles_.emplace_back(std::move(handle));

      // Add the accessor to the object template
      objectTemplate->SetAccessor(v8::String::NewFromUtf8(isolate, name).ToLocalChecked(),
                                  GetterWrapper,
                                  nullptr,
                                  dataValue,
                                  v8::AccessControl::DEFAULT,
                                  attributes);
    }
    /**
     * Create a standardized property accessor for given object template. This is same to define getter/setter at 
     * JavaScript side via:
     * 
     * ```
     * class Foo {
     *   get bar() { ...}
     * }
     * ```
     * 
     * @param isolate The v8::Isolate instance.
     * @param tpl The object template to which the property accessor will be added.
     * @param name The name of the property.
     * @param getterCallback The getter callback function.
     * @param setterCallback The setter callback function.
     * @param attributes The property attributes.
     */
    static void InstancePropertyAccessor(v8::Isolate *isolate,
                                         v8::Local<v8::ObjectTemplate> tpl,
                                         const char *name,
                                         InstanceFunctionCallback getterCallback,
                                         InstanceFunctionCallback setterCallback,
                                         v8::PropertyAttribute attributes = v8::PropertyAttribute::None)
    {
      auto releaseCallback = [](const v8::WeakCallbackInfo<InstanceFunctionCallbackData> &data)
      {
        InstanceFunctionCallbackData *callbackData = data.GetParameter();
        if (callbackData != nullptr)
          delete callbackData;
      };

      v8::Local<v8::FunctionTemplate> getterTpl = v8::Local<v8::FunctionTemplate>();
      if (getterCallback)
      {
        auto callbackData = new InstanceFunctionCallbackData{getterCallback, nullptr};
        auto dataValue = v8::External::New(isolate, callbackData);
        {
          auto handle = std::make_unique<v8::Persistent<v8::External>>(isolate, dataValue);
          handle->SetWeak(callbackData, releaseCallback, v8::WeakCallbackType::kParameter);
          T::callback_data_handles_.emplace_back(std::move(handle));
        }

        std::string funcName = std::string("get ") + name;
        getterTpl = v8::FunctionTemplate::New(isolate,
                                              PropertyGetterWrapper,
                                              dataValue,
                                              v8::Local<v8::Signature>(),
                                              0,
                                              v8::ConstructorBehavior::kThrow,
                                              v8::SideEffectType::kHasNoSideEffect);
        getterTpl->SetClassName(v8::String::NewFromUtf8(isolate, funcName.c_str()).ToLocalChecked());
      }

      v8::Local<v8::FunctionTemplate> setterTpl = v8::Local<v8::FunctionTemplate>();
      if (setterCallback)
      {
        auto callbackData = new InstanceFunctionCallbackData{setterCallback, nullptr};
        auto dataValue = v8::External::New(isolate, callbackData);
        {
          auto handle = std::make_unique<v8::Persistent<v8::External>>(isolate, dataValue);
          handle->SetWeak(callbackData, releaseCallback, v8::WeakCallbackType::kParameter);
          T::callback_data_handles_.emplace_back(std::move(handle));
        }

        std::string funcName = std::string("set ") + name;
        setterTpl = v8::FunctionTemplate::New(isolate,
                                              PropertySetterWrapper,
                                              dataValue,
                                              v8::Local<v8::Signature>(),
                                              1,
                                              v8::ConstructorBehavior::kThrow,
                                              v8::SideEffectType::kHasSideEffect);
        setterTpl->SetClassName(v8::String::NewFromUtf8(isolate, funcName.c_str()).ToLocalChecked());
      }

      tpl->SetAccessorProperty(v8::String::NewFromUtf8(isolate, name).ToLocalChecked(),
                               getterTpl,
                               setterTpl,
                               attributes);
    }
    /**
     * A convenience method to create a read-only property accessor.
     * 
     * @param isolate The v8::Isolate instance.
     * @param tpl The object template to which the property accessor will be added.
     * @param name The name of the property.
     * @param getter The getter callback function.
     * @param attributes The property attributes.
     */
    static void InstanceReadonlyPropertyAccessor(v8::Isolate *isolate,
                                                 v8::Local<v8::ObjectTemplate> tpl,
                                                 const char *name,
                                                 InstanceFunctionCallback getter,
                                                 v8::PropertyAttribute attributes = v8::PropertyAttribute::ReadOnly)
    {
      InstancePropertyAccessor(isolate, tpl, name, getter, nullptr, attributes);
    }

    /**
     * Create a standardized static method callback for the class.
     * 
     * @param isolate The v8::Isolate instance.
     * @param tpl The function template to which the static method will be added.
     * @param name The name of the static method.
     * @param callback The static method callback function.
     */
    static void StaticMethod(v8::Isolate *isolate,
                             v8::Local<v8::FunctionTemplate> tpl,
                             const char *name,
                             v8::FunctionCallback callback)
    {
      v8::HandleScope scope(isolate);
      tpl->Set(v8::String::NewFromUtf8(isolate, name).ToLocalChecked(),
               v8::FunctionTemplate::New(isolate, callback));
    }

    /// Error creation helpers

    /**
     * Create a standardized error message for method failures.
     *
     * @param isolate The v8::Isolate instance.
     * @param method The name of the method where the error occurred.
     * @param message The error message describing the failure.
     * @returns A formatted error message string.
     */
    static v8::Local<v8::String> ErrorMessage(v8::Isolate *isolate, const char *method, const char *message)
    {
      std::string str = "Failed to execute '" + std::string(method) + "' on '" + T::Name() + "': " +
                        std::string(message);
      return v8::String::NewFromUtf8(isolate, str.c_str()).ToLocalChecked();
    }
    /**
     * Create a standardized error message for method failures.
     * 
     * @param isolate The v8::Isolate instance.
     * @param method The name of the method where the error occurred.
     * @param message The error message describing the failure.
     * @returns A formatted error message string.
     */
    static v8::Local<v8::String> MakeMethodError(v8::Isolate *isolate, const char *method, const char *message)
    {
      std::string str = "Failed to execute '" + std::string(method) + "' on '" + T::Name() + "': " +
                        std::string(message);
      return v8::String::NewFromUtf8(isolate, str.c_str()).ToLocalChecked();
    }
    /**
     * Create a standardized error message for method argument count errors.
     * 
     * @param isolate The v8::Isolate instance.
     * @param method The name of the method where the error occurred.
     * @param expected The expected number of arguments.
     * @param actual The actual number of arguments provided.
     */
    static v8::Local<v8::String> MakeMethodArgCountError(v8::Isolate *isolate,
                                                         const char *method,
                                                         int expected,
                                                         int actual)
    {
      std::string str = std::to_string(expected) + " argument(s) required, " +
                        "but only " + std::to_string(actual) + " present.";
      return MakeMethodError(isolate, method, str.c_str());
    }
    /**
     * Create a standardized error message for method argument type errors.
     * 
     * @param isolate The v8::Isolate instance.
     * @param method The name of the method where the error occurred.
     * @param index The index of the argument that has the wrong type.
     * @param expectedType The expected type of the argument.
     * @param arg The actual argument value.
     */
    static v8::Local<v8::String> MakeMethodArgTypeError(v8::Isolate *isolate,
                                                        const char *method,
                                                        int index,
                                                        const char *expectedType,
                                                        v8::Local<v8::Value> arg)
    {
      v8::String::Utf8Value argUtf8(isolate, arg);
      std::string str = "Argument " + std::to_string(index) + " must be of type " + std::string(expectedType) + ", " +
                        "but got '" + std::string(*argUtf8) + "'.";
      return MakeMethodError(isolate, method, str.c_str());
    }
    /**
     * Create a standardized error message for method argument type errors.
     * 
     * @param isolate The v8::Isolate instance.
     * @param method The name of the method where the error occurred.
     * @param name The name of the argument that has the wrong type.
     * @param expectedType The expected type of the argument.
     * @param arg The actual argument value.
     */
    static v8::Local<v8::String> MakeMethodArgTypeError(v8::Isolate *isolate,
                                                        const char *method,
                                                        const char *name,
                                                        const char *expectedType,
                                                        v8::Local<v8::Value> arg)
    {
      if (arg->IsObject())
      {
        std::cerr << "Argument '" << name << "' is an object:" << std::endl;
        Warn(isolate, arg); // Log the actual argument value for debugging
      }

      v8::String::Utf8Value argUtf8(isolate, arg);
      std::string str = "Argument '" + std::string(name) + "' must be of type " + std::string(expectedType) + ", " +
                        "but got '" + std::string(*argUtf8) + "'.";
      return MakeMethodError(isolate, method, str.c_str());
    }
    /**
     * Create a standardized error message for constructor failures.
     * 
     * @param isolate The v8::Isolate instance.
     * @param message The error message describing the failure.
     * @returns A formatted error message string.
     */
    static v8::Local<v8::String> MakeConstructorError(v8::Isolate *isolate, const char *message)
    {
      std::string str = "Failed to construct '" + T::Name() + "': " + std::string(message);
      return v8::String::NewFromUtf8(isolate, str.c_str()).ToLocalChecked();
    }
    /**
     * Use current global's `console` object to log the value.
     * 
     * @param isolate The v8::Isolate instance.
     * @param method The console method to use, e.g. "log", "warn", "error".
     * @param value The value to log.
     */
    static void Log(v8::Isolate *isolate, const char *method, v8::Local<v8::Value> value)
    {
      v8::HandleScope handleScope(isolate);
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::Context::Scope contextScope(context);

      v8::Local<v8::String> consoleKey = v8::String::NewFromUtf8(isolate, "console").ToLocalChecked();
      v8::Local<v8::Value> consoleValue;
      v8::Local<v8::Object> global = context->Global();

      if (global->Get(context, consoleKey).ToLocal(&consoleValue) &&
          consoleValue->IsObject())
      {
        v8::Local<v8::Object> console = consoleValue.As<v8::Object>();
        v8::Local<v8::Value> methodValue = console->Get(context,
                                                        v8::String::NewFromUtf8(isolate, method).ToLocalChecked())
                                             .ToLocalChecked();

        if (methodValue->IsFunction())
        {
          v8::Local<v8::Function> logFunction = methodValue.As<v8::Function>();

          if (value.IsEmpty())
          {
            v8::Local<v8::Value> args[] = {Undefined(isolate)};
            logFunction->Call(context, console, 1, args).ToLocalChecked();
          }
          else
          {
            v8::Local<v8::Value> args[] = {value};
            logFunction->Call(context, console, 1, args).ToLocalChecked();
          }
          return;
        }
      }

      // Fallback: console.log is not available
      assert(false && "console.[method] is not available");
    }
    static inline void Log(v8::Isolate *isolate, v8::Local<v8::Value> value)
    {
      Log(isolate, "log", value);
    }
    static inline void Warn(v8::Isolate *isolate, v8::Local<v8::Value> value)
    {
      Log(isolate, "warn", value);
    }
    static inline void Error(v8::Isolate *isolate, v8::Local<v8::Value> value)
    {
      Log(isolate, "error", value);
    }
    static inline void Trace(v8::Isolate *isolate, v8::Local<v8::Value> value)
    {
      Log(isolate, "trace", value);
    }

  private:
    template <typename TCallbackData>
    static inline TCallbackData *GetCallbackData(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      return GetCallbackData<TCallbackData>(info.GetIsolate(), info.Data());
    }
    template <typename TCallbackData>
    static inline TCallbackData *GetCallbackData(const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      return GetCallbackData<TCallbackData>(info.GetIsolate(), info.Data());
    }
    template <typename TCallbackData>
    static inline TCallbackData *GetCallbackData(const v8::PropertyCallbackInfo<void> &info)
    {
      return GetCallbackData<TCallbackData>(info.GetIsolate(), info.Data());
    }
    template <typename TCallbackData>
    static TCallbackData *GetCallbackData(v8::Isolate *isolate, v8::Local<v8::Value> data)
    {
      v8::HandleScope scope(isolate);
      assert(data->IsExternal() && "data must be an External");

      auto external = data.As<v8::External>();
      assert(external->Value() != nullptr && "callback data must not be null");

      TCallbackData *callbackData = static_cast<TCallbackData *>(external->Value());
      if (callbackData == nullptr) [[unlikely]]
      {
        isolate->ThrowException(v8::Exception::TypeError(
          v8::String::NewFromUtf8(isolate, "Illegal invocation").ToLocalChecked()));
        return nullptr;
      }
      return callbackData;
    }

    static void MethodWrapper(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      v8::HandleScope scope(isolate);

      auto callbackData = T::template GetCallbackData<InstanceFunctionCallbackData>(info);
      if (callbackData != nullptr)
      {
        assert(callbackData->callback != nullptr && "callback must not be null");

        T *instance = T::Unwrap(isolate, info.This());
        if (instance == nullptr) [[unlikely]]
        {
          isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Illegal invocation").ToLocalChecked()));
          return;
        }

        // Call the method callback
        std::bind(callbackData->callback, instance, std::placeholders::_1)(info);
        return;
      }
    }

    static void GetterWrapper(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      v8::HandleScope scope(isolate);

      auto callbackData = T::template GetCallbackData<InstanceAccessorCallbackData>(info);
      if (callbackData != nullptr && callbackData->getterCallback) [[likely]]
      {
        T *instance = T::Unwrap(isolate, info.This());
        if (instance == nullptr) [[unlikely]]
        {
          isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Illegal invocation").ToLocalChecked()));
          return;
        }

        // Call the getter callback
        std::bind(callbackData->getterCallback,
                  instance,
                  std::placeholders::_1)(info);
        return;
      }

      // No getter defined, return undefined
      info.GetReturnValue().Set(v8::Undefined(isolate));
    }

    static void SetterWrapper(v8::Local<v8::String> property,
                              v8::Local<v8::Value> value,
                              const v8::PropertyCallbackInfo<void> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      v8::HandleScope scope(isolate);

      auto callbackData = T::template GetCallbackData<InstanceAccessorCallbackData>(info);
      if (callbackData != nullptr && callbackData->setterCallback) [[likely]]
      {
        T *instance = T::Unwrap(isolate, info.This());
        if (instance == nullptr) [[unlikely]]
        {
          isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Illegal invocation").ToLocalChecked()));
          return;
        }

        // Call the setter callback
        std::bind(callbackData->setterCallback,
                  instance,
                  std::placeholders::_1,
                  std::placeholders::_2)(value, info);
        return;
      }
    }

    static void PropertyGetterWrapper(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      v8::HandleScope scope(isolate);

      auto callbackData = T::template GetCallbackData<InstanceFunctionCallbackData>(info);
      if (callbackData != nullptr)
      {
        assert(callbackData->callback != nullptr && "callback must not be null");

        T *instance = T::Unwrap(isolate, info.This());
        if (instance == nullptr) [[unlikely]]
        {
          isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Illegal invocation").ToLocalChecked()));
          return;
        }

        // Call the method callback
        std::bind(callbackData->callback, instance, std::placeholders::_1)(info);
        return;
      }
    }

    static void PropertySetterWrapper(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      v8::HandleScope scope(isolate);

      if (info.Length() < 1) [[unlikely]]
      {
        isolate->ThrowException(v8::Exception::TypeError(
          v8::String::NewFromUtf8(isolate, "Failed to set property: 1 argument required, but only 0 present.")
            .ToLocalChecked()));
        return;
      }

      // Setter does not return a value
      info.GetReturnValue().SetUndefined();

      // Call the actual setter
      auto callbackData = T::template GetCallbackData<InstanceFunctionCallbackData>(info);
      if (callbackData != nullptr)
      {
        assert(callbackData->callback != nullptr && "callback must not be null");

        T *instance = T::Unwrap(isolate, info.This());
        if (instance == nullptr) [[unlikely]]
        {
          isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Illegal invocation").ToLocalChecked()));
          return;
        }

        // Call the method callback
        std::bind(callbackData->callback, instance, std::placeholders::_1)(info);
        return;
      }
    }

  public:
    /**
     * Constructing an object without JavaScript arguments.
     */
    ObjectWrap(v8::Isolate *isolate)
        : TBase(isolate)
    {
    }

    /**
     * Constructor for ObjectWrap.
     * 
     * @param isolate The v8::Isolate instance.
     * @param args The function callback info containing the arguments.
     */
    ObjectWrap(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
        : TBase(isolate, args)
    {
    }

    virtual ~ObjectWrap()
    {
    }

    static void Constructor(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
      v8::Isolate *isolate = args.GetIsolate();
      v8::HandleScope scope(isolate);

      if (!args.IsConstructCall())
      {
        isolate->ThrowException(v8::Exception::TypeError(MakeConstructorError(isolate,
                                                                              "Illegal constructor")));
        return;
      }

      ConstructingContext *constructingContext = nullptr;
      if (args.Length() == 1 && args[0]->IsExternal())
      {
        v8::Local<v8::External> firstArg = args[0].As<v8::External>();
        assert(firstArg->Value() != nullptr && "option must not be null");
        constructingContext = static_cast<ConstructingContext *>(firstArg->Value());
      }

      T *instance = nullptr;
      if (constructingContext == nullptr ||
          constructingContext->isScriptCall())
      {
        // If `NativeConstructorRequired` is true, only allow native construction
        if (T::NativeConstructorRequired())
        {
          isolate->ThrowException(v8::Exception::TypeError(MakeConstructorError(isolate,
                                                                                "Illegal constructor")));
          return;
        }
        instance = new T(isolate, args);
        instance->onCreated();
      }
      else
      {
        // Native constructing will use the default constructor, and call `onCreated` later
        instance = new T(isolate);
      }
      assert(instance != nullptr && "Failed to create instance");

      // Delete the constructing context if it was created
      if (constructingContext != nullptr)
        delete constructingContext;

      auto jsThis = args.This();
      Wrap(isolate, jsThis, instance);

      args.GetReturnValue().Set(jsThis);
    }

  public:
    /**
     * @returns The `ObjectWrap`'s V8 value.
     */
    v8::Local<v8::Value> value() const
    {
      return this->object_handle_.Get(this->current_isolate_);
    }

    /**
     * Get the data handle as a specific type.
     * 
     * @tparam U The type to cast the data handle to.
     * @returns The data handle cast to the specified type.
     */
    template <typename U = TData>
    inline std::shared_ptr<U> handle() const
    {
      static_assert(!std::is_same_v<U, void>, "U must not be void");
      std::shared_ptr<U> data_handle = std::dynamic_pointer_cast<U>(this->data_handle_);
      if (data_handle == nullptr) [[unlikely]]
      {
        std::cerr << "Error: failed to get data handle of '" << T::Name() << "' as '" << typeid(U).name() << "'."
                  << std::endl;
        assert(false && "`data_handle` is null");
      }
      return data_handle;
    }

  public:
    static thread_local inline bool initialized_ = false;
    static thread_local inline v8::Persistent<v8::FunctionTemplate> function_template_;
    static thread_local inline v8::Persistent<v8::Function> constructor_handle_;
    static thread_local inline std::vector<std::unique_ptr<v8::Persistent<v8::External>>> callback_data_handles_{};
  };
}
