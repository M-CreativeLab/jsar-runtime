#include <client/scripting_base/v8_utils.hpp>

#include "./intersection_observer.hpp"
#include "./element.hpp"
#include "./node-inl.hpp"

namespace dombinding
{
  using namespace std;

  thread_local Napi::FunctionReference *IntersectionObserver::constructor = nullptr;

  void IntersectionObserver::Init(Napi::Env env)
  {
#define MODULE_NAME "IntersectionObserver"
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
      env,
      MODULE_NAME,
      {
        InstanceMethod("disconnect", &IntersectionObserver::Disconnect),
        InstanceMethod("observe", &IntersectionObserver::Observe),
        InstanceMethod("unobserve", &IntersectionObserver::Unobserve),
        InstanceMethod("takeRecords", &IntersectionObserver::TakeRecords),
        InstanceAccessor("root", &IntersectionObserver::GetRoot, nullptr),
        InstanceAccessor("rootMargin", &IntersectionObserver::GetRootMargin, nullptr),
        InstanceAccessor("thresholds", &IntersectionObserver::GetThresholds, nullptr),
      });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    (*constructor).SuppressDestruct();

    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }

  void IntersectionObserver::MicrotaskHandler(v8::Isolate *isolate, void *data)
  {
    if (TR_LIKELY(data != nullptr))
    {
      IntersectionObserver *observer = reinterpret_cast<IntersectionObserver *>(data);
      observer->dispatchEntriesCallback(observer->callbackEnv_);
    }
  }

  IntersectionObserver::IntersectionObserver(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<IntersectionObserver>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0 || !info[0].IsFunction())
    {
      string msg = "Failed to construct 'IntersectionObserver': 1 argument required, but only 0 present.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return;
    }

    Napi::Function callback = info[0].As<Napi::Function>();
    callbackRef_.Reset(callback);

    // Parse options if provided
    dom::IntersectionObserver::ObserveOptions options;
    if (info.Length() >= 2 && info[1].IsObject())
    {
      Napi::Object optionsObject = info[1].As<Napi::Object>();

      // Parse root
      if (optionsObject.Has("root") && !optionsObject.Get("root").IsNull())
      {
        Napi::Object rootObject = optionsObject.Get("root").As<Napi::Object>();
        // Get the Element implementation from the JS object
        // For now, we'll leave this as nullptr to use viewport
        options.root = nullptr;
      }

      // Parse rootMargin
      if (optionsObject.Has("rootMargin"))
      {
        options.rootMargin = optionsObject.Get("rootMargin").ToString().Utf8Value();
      }

      // Parse threshold
      if (optionsObject.Has("threshold"))
      {
        Napi::Value thresholdValue = optionsObject.Get("threshold");
        options.threshold.clear();

        if (thresholdValue.IsArray())
        {
          Napi::Array thresholdArray = thresholdValue.As<Napi::Array>();
          for (size_t i = 0; i < thresholdArray.Length(); i++)
          {
            double threshold = thresholdArray.Get(i).ToNumber().DoubleValue();
            options.threshold.push_back(threshold);
          }
        }
        else if (thresholdValue.IsNumber())
        {
          double threshold = thresholdValue.ToNumber().DoubleValue();
          options.threshold.push_back(threshold);
        }
      }
    }

    observer_ = std::make_shared<dom::IntersectionObserver>(nullptr, options);
  }

  IntersectionObserver::~IntersectionObserver()
  {
    stopEntriesDispatcher();
  }

  Napi::Value IntersectionObserver::Disconnect(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    stopEntriesDispatcher();
    if (observer_ == nullptr)
    {
      Napi::Error::New(env, "The intersection observer is already disconnected.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    observer_->disconnect();
    observer_.reset();
    return env.Undefined();
  }

  Napi::Value IntersectionObserver::Observe(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (observer_ == nullptr)
    {
      Napi::Error::New(env, "The intersection observer is already disconnected.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() == 0 || !info[0].IsObject())
    {
      Napi::TypeError::New(env, "A target element is required.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Object target = info[0].As<Napi::Object>();

    // Get the Element implementation from the JS object
    shared_ptr<dom::Node> targetNode = Node::GetImpl(target);
    if (targetNode == nullptr)
    {
      Napi::TypeError::New(env, "The target must be a valid Node object.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    // Cast to Element
    shared_ptr<dom::Element> targetElement = dynamic_pointer_cast<dom::Element>(targetNode);
    if (targetElement == nullptr)
    {
      Napi::TypeError::New(env, "The target must be a valid Element object.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    observer_->observe(targetElement);
    startEntriesDispatcher(env);
    return env.Undefined();
  }

  Napi::Value IntersectionObserver::Unobserve(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (observer_ == nullptr)
    {
      Napi::Error::New(env, "The intersection observer is already disconnected.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() == 0 || !info[0].IsObject())
    {
      Napi::TypeError::New(env, "A target element is required.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Object target = info[0].As<Napi::Object>();

    // Get the Element implementation from the JS object
    shared_ptr<dom::Node> targetNode = Node::GetImpl(target);
    if (targetNode == nullptr)
    {
      Napi::TypeError::New(env, "The target must be a valid Node object.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    // Cast to Element
    shared_ptr<dom::Element> targetElement = dynamic_pointer_cast<dom::Element>(targetNode);
    if (targetElement == nullptr)
    {
      Napi::TypeError::New(env, "The target must be a valid Element object.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    observer_->unobserve(targetElement);
    return env.Undefined();
  }

  Napi::Value IntersectionObserver::TakeRecords(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (observer_ == nullptr)
    {
      Napi::Error::New(env, "The intersection observer is already disconnected.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    vector<dom::IntersectionObserverEntry> entries = observer_->takeRecords();
    Napi::Array entriesArray = Napi::Array::New(env, entries.size());

    for (size_t i = 0; i < entries.size(); i++)
    {
      // Create JS object for IntersectionObserverEntry
      Napi::Object entryObj = Napi::Object::New(env);
      const auto &entry = entries[i];

      // Set properties
      entryObj.Set("intersectionRatio", Napi::Number::New(env, entry.intersectionRatio));
      entryObj.Set("isIntersecting", Napi::Boolean::New(env, entry.isIntersecting));
      entryObj.Set("time", Napi::Number::New(env, entry.time));

      // Create DOMRect objects
      Napi::Object boundingClientRect = Napi::Object::New(env);
      boundingClientRect.Set("x", Napi::Number::New(env, entry.boundingClientRect.x()));
      boundingClientRect.Set("y", Napi::Number::New(env, entry.boundingClientRect.y()));
      boundingClientRect.Set("width", Napi::Number::New(env, entry.boundingClientRect.width()));
      boundingClientRect.Set("height", Napi::Number::New(env, entry.boundingClientRect.height()));
      entryObj.Set("boundingClientRect", boundingClientRect);

      Napi::Object intersectionRect = Napi::Object::New(env);
      intersectionRect.Set("x", Napi::Number::New(env, entry.intersectionRect.x()));
      intersectionRect.Set("y", Napi::Number::New(env, entry.intersectionRect.y()));
      intersectionRect.Set("width", Napi::Number::New(env, entry.intersectionRect.width()));
      intersectionRect.Set("height", Napi::Number::New(env, entry.intersectionRect.height()));
      entryObj.Set("intersectionRect", intersectionRect);

      if (entry.rootBounds.has_value())
      {
        Napi::Object rootBounds = Napi::Object::New(env);
        rootBounds.Set("x", Napi::Number::New(env, entry.rootBounds->x()));
        rootBounds.Set("y", Napi::Number::New(env, entry.rootBounds->y()));
        rootBounds.Set("width", Napi::Number::New(env, entry.rootBounds->width()));
        rootBounds.Set("height", Napi::Number::New(env, entry.rootBounds->height()));
        entryObj.Set("rootBounds", rootBounds);
      }
      else
      {
        entryObj.Set("rootBounds", env.Null());
      }

      // TODO: Set target element reference
      entryObj.Set("target", env.Null());

      entriesArray.Set(i, entryObj);
    }

    return entriesArray;
  }

  Napi::Value IntersectionObserver::GetRoot(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (observer_ == nullptr)
      return env.Null();

    auto root = observer_->getRoot();
    if (root == nullptr)
      return env.Null();

    // TODO: Convert Element to JS object
    return env.Null();
  }

  Napi::Value IntersectionObserver::GetRootMargin(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (observer_ == nullptr)
      return env.Undefined();

    return Napi::String::New(env, observer_->getRootMargin());
  }

  Napi::Value IntersectionObserver::GetThresholds(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (observer_ == nullptr)
      return env.Undefined();

    const auto &thresholds = observer_->getThreshold();
    Napi::Array array = Napi::Array::New(env, thresholds.size());

    for (size_t i = 0; i < thresholds.size(); i++)
    {
      array.Set(i, Napi::Number::New(env, thresholds[i]));
    }

    return array;
  }

  void IntersectionObserver::dispatchEntriesCallback(Napi::Env env)
  {
    vector<dom::IntersectionObserverEntry> entries = observer_->takeRecords();
    if (entries.size() > 0)
    {
      Napi::HandleScope scope(env);
      Napi::Function callback = callbackRef_.Value();
      Napi::Array entriesArray = Napi::Array::New(env, entries.size());

      for (size_t i = 0; i < entries.size(); i++)
      {
        // Create JS object for IntersectionObserverEntry (similar to TakeRecords)
        Napi::Object entryObj = Napi::Object::New(env);
        const auto &entry = entries[i];

        entryObj.Set("intersectionRatio", Napi::Number::New(env, entry.intersectionRatio));
        entryObj.Set("isIntersecting", Napi::Boolean::New(env, entry.isIntersecting));
        entryObj.Set("time", Napi::Number::New(env, entry.time));

        // Create DOMRect objects
        Napi::Object boundingClientRect = Napi::Object::New(env);
        boundingClientRect.Set("x", Napi::Number::New(env, entry.boundingClientRect.x()));
        boundingClientRect.Set("y", Napi::Number::New(env, entry.boundingClientRect.y()));
        boundingClientRect.Set("width", Napi::Number::New(env, entry.boundingClientRect.width()));
        boundingClientRect.Set("height", Napi::Number::New(env, entry.boundingClientRect.height()));
        entryObj.Set("boundingClientRect", boundingClientRect);

        Napi::Object intersectionRect = Napi::Object::New(env);
        intersectionRect.Set("x", Napi::Number::New(env, entry.intersectionRect.x()));
        intersectionRect.Set("y", Napi::Number::New(env, entry.intersectionRect.y()));
        intersectionRect.Set("width", Napi::Number::New(env, entry.intersectionRect.width()));
        intersectionRect.Set("height", Napi::Number::New(env, entry.intersectionRect.height()));
        entryObj.Set("intersectionRect", intersectionRect);

        if (entry.rootBounds.has_value())
        {
          Napi::Object rootBounds = Napi::Object::New(env);
          rootBounds.Set("x", Napi::Number::New(env, entry.rootBounds->x()));
          rootBounds.Set("y", Napi::Number::New(env, entry.rootBounds->y()));
          rootBounds.Set("width", Napi::Number::New(env, entry.rootBounds->width()));
          rootBounds.Set("height", Napi::Number::New(env, entry.rootBounds->height()));
          entryObj.Set("rootBounds", rootBounds);
        }
        else
        {
          entryObj.Set("rootBounds", env.Null());
        }

        entryObj.Set("target", env.Null()); // TODO: Set actual target
        entriesArray.Set(i, entryObj);
      }

      try
      {
        callback.Call(env.Undefined(), {entriesArray, this->Value()});
      }
      catch (Napi::Error &e)
      {
        std::cerr << "Failed to dispatch callback on 'IntersectionObserver': "
                  << e.Message() << std::endl;
      }
      catch (std::exception &e)
      {
        std::cerr << "Failed to dispatch callback on 'IntersectionObserver': "
                  << "std::exception(" << e.what() << ")" << std::endl;
      }
    }
  }

  void IntersectionObserver::startEntriesDispatcher(Napi::Env env)
  {
    callbackEnv_ = env;

    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    isolate->AddMicrotasksCompletedCallback(MicrotaskHandler, this);
  }

  void IntersectionObserver::stopEntriesDispatcher()
  {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    isolate->RemoveMicrotasksCompletedCallback(MicrotaskHandler, this);
  }
}