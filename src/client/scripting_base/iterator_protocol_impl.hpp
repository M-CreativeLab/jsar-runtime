#pragma once

#include <memory>
#include <vector>
#include <string>
#include <node/v8.h>

#include "./v8_object_wrap.hpp"

namespace scripting_base
{
  /**
   * A base class for implementing the Iterator Protocol in V8.
   *
   * @tparam T The class to wrap
   * @tparam ValueType The type of the values in the data source
   */
  template <typename T, typename ValueType>
  class Iterator : public ObjectWrap<T>
  {
  public:
    static std::string Name()
    {
      return "Iterator";
    }

    // Creates a new instance of the Iterator class, with a vector of data source.
    static v8::Local<v8::Value> NewInstance(napi_env napiEnv,
                                            const std::vector<std::shared_ptr<ValueType>> &dataSource)
    {
      v8::Isolate *isolate = v8::Isolate::GetCurrent();
      v8::EscapableHandleScope scope(isolate);
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      v8::Local<v8::Value> jsValue = ObjectWrap<T>::NewInstance(napiEnv);
      v8::Local<v8::Object> jsObject = jsValue->ToObject(context).ToLocalChecked();
      T *instance = ObjectWrap<T>::Unwrap(jsObject);
      assert(instance != nullptr && "instance must not be null");

      instance->dataSource_ = dataSource;
      return scope.Escape(jsValue);
    }

  public:
    Iterator(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &info)
        : ObjectWrap<T>(isolate, info)
    {
      v8::HandleScope scope(isolate);
      auto context = isolate->GetCurrentContext();
      auto jsObject = info.This();

      // Configure the Iterator Protocol
      jsObject->Set(context,
                    v8::String::NewFromUtf8Literal(isolate, "next"),
                    v8::FunctionTemplate::New(isolate, Next)->GetFunction(context).ToLocalChecked())
        .FromJust();
      jsObject->Set(context,
                    v8::String::NewFromUtf8Literal(isolate, "return"),
                    v8::FunctionTemplate::New(isolate, Return)->GetFunction(context).ToLocalChecked())
        .FromJust();
      jsObject->Set(context,
                    v8::String::NewFromUtf8Literal(isolate, "throw"),
                    v8::FunctionTemplate::New(isolate, Throw)->GetFunction(context).ToLocalChecked())
        .FromJust();
    }
    virtual ~Iterator() = default;

  protected:
    virtual v8::Local<v8::Value> createNextValue(v8::Isolate *isolate, const std::shared_ptr<ValueType> value)
    {
      return v8::Undefined(isolate);
    }

  private:
    /**
     * The next() method returns an object with two properties done and value.
     *
     * @param info The callback info
     */
    static void Next(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::HandleScope scope(isolate);

      T *instance = ObjectWrap<T>::Unwrap(info.This());
      if (!instance)
      {
        info.GetReturnValue().Set(v8::Undefined(isolate));
        return;
      }

      // Return { done: true } if the iterator has been consumed or the data source is empty
      if (instance->hasDone_ || instance->dataSource_.empty())
      {
        v8::Local<v8::Object> result = v8::Object::New(isolate);
        result->Set(context, v8::String::NewFromUtf8Literal(isolate, "done"), v8::Boolean::New(isolate, true))
          .FromJust();
        info.GetReturnValue().Set(result);
        return;
      }

      auto value = instance->dataSource_.front();
      instance->dataSource_.erase(instance->dataSource_.begin());

      v8::Local<v8::Object> result = v8::Object::New(isolate);
      result->Set(context, v8::String::NewFromUtf8Literal(isolate, "value"), instance->createNextValue(isolate, value))
        .FromJust();
      result->Set(context, v8::String::NewFromUtf8Literal(isolate, "done"), v8::Boolean::New(isolate, false))
        .FromJust();
      info.GetReturnValue().Set(result);
    }

    /**
     * The return() method returns the given value and finishes the iterator.
     */
    static void Return(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::HandleScope scope(isolate);
      std::cerr << "Iterator::Return()" << std::endl;

      info.GetReturnValue().Set(v8::Undefined(isolate));
    }

    /**
     * The throw() method throws an error into the generator.
     */
    static void Throw(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::HandleScope scope(isolate);
      std::cerr << "Iterator::Throw()" << std::endl;

      info.GetReturnValue().Set(v8::Undefined(isolate));
    }

  private:
    std::vector<std::shared_ptr<ValueType>> dataSource_;
    bool hasDone_ = false;
  };

  /**
   * A base class for implementing the Iterable Protocol in V8.
   *
   * @tparam T The class to wrap
   * @tparam ValueType The type of the values in the data source
   */
  template <typename T, typename ValueType>
  class Iterable : public Iterator<T, ValueType>
  {
  public:
    /**
     * Creates a new instance of the Iterable class.
     *
     * @param isolate The V8 isolate
     * @param info The callback info
     */
    Iterable(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &info)
        : Iterator<T, ValueType>(isolate, info)
    {
      v8::HandleScope scope(isolate);
      auto context = isolate->GetCurrentContext();
      auto jsObject = info.This();

      // Get the Symbol.iterator value
      jsObject->Set(context,
                    v8::Symbol::GetIterator(isolate),
                    v8::FunctionTemplate::New(isolate, GetIterator)->GetFunction(context).ToLocalChecked())
        .FromJust();
    }

  private:
    /**
     * The @@iterator method returns the iterator object itself.
     */
    static void GetIterator(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::HandleScope scope(isolate);
      info.GetReturnValue().Set(info.This());
    }
  };
}
