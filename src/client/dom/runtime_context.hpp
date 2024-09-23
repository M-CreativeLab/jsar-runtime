#pragma once

#include <functional>
#include <string>
#include <memory>
#include <v8.h>

#include "./dom_parser.hpp"
#include "./dom_scripting.hpp"

namespace dom
{
  typedef std::function<void(const v8::FunctionCallbackInfo<v8::Value> &)> ResponseCallback;
  typedef std::function<void(const std::string &)> StringResponseCallback;
  typedef std::function<void(const std::string &)> ImageResponseCallback;

  class RuntimeContext : public std::enable_shared_from_this<RuntimeContext>
  {
  public:
    RuntimeContext() : isolate(v8::Isolate::GetCurrent())
    {
    }
    virtual ~RuntimeContext() {}

  public:
    /**
     * Initialize the fields before using this runtime context.
     */
    virtual void initialize();

    /**
     * Set the base URI value.
     * 
     * @param baseURI The base URI value to set.
     */
    void setBaseURI(const std::string baseURI);

    /**
     * Set the resource loader value.
     *
     * @param value The `ResourceLoader` value to set.
     */
    void setResourceLoaderValue(v8::Local<v8::Value> value);

    /**
     * Fetch the resource from the given URL.
     *
     * @param url The URL of the resource to fetch.
     * @param responseType The type of the response to expect: "string", "arraybuffer" or "json".
     * @param callback The callback to call when the resource is fetched.
     */
    void fetchResource(const std::string &url, const std::string &responseType, const ResponseCallback &callback);

    /**
     * Fetch the text source type of the resource from the given URL.
     *
     * @param url The URL of the resource to fetch.
     * @param callback The callback to call when the resource is fetched.
     */
    void fetchTextSourceResource(const std::string &url, const StringResponseCallback &callback);

    /**
     * Fetch the image resource from the given URL.
     * 
     * @param url The URL of the image to fetch.
     * @param callback The callback to call when the image is fetched.
     */
    void fetchImageResource(const std::string &url, const ImageResponseCallback &callback);

    /**
     * Create the WHATWG `fetch` function with the given base URI.
     *
     * @param context The context to create the function in.
     */
    v8::Local<v8::Value> createWHATWGFetchImpl(v8::Local<v8::Context> context);

  protected:
    /**
     * Get the shared pointer of the runtime context.
     *
     * @returns The shared pointer of the runtime context.
     */
    template <typename T = RuntimeContext>
    shared_ptr<T> getSharedPtr()
    {
      return dynamic_pointer_cast<T>(shared_from_this());
    }

  public:
    std::string baseURI;
    shared_ptr<DOMScriptingContext> scriptingContext;

  protected:
    v8::Isolate *isolate;
    v8::Global<v8::Object> resourceLoaderValue;
  };
}
