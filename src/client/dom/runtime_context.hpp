#pragma once

#include <string>
#include <memory>
#include <thread>
#include <functional>
#include <deque>
#include <shared_mutex>
#include <v8.h>
#include <node/uv.h>

#include "./dom_parser.hpp"
#include "./dom_scripting.hpp"

namespace endor
{
  namespace dom
  {
    typedef std::function<void(const v8::FunctionCallbackInfo<v8::Value> &)> FunctionCallback;
    typedef std::function<void(const std::string &)> ErrorCallback;
    typedef std::function<void(const std::string &)> StringResponseCallback;
    typedef std::function<void(const void *, size_t)> BufferResponseCallback;

    class RuntimeContext : public std::enable_shared_from_this<RuntimeContext>
    {
    private:
      class FetchRequest
      {
      public:
        FetchRequest(const std::string &url,
                     const std::string &responseType,
                     const FunctionCallback &successCallback,
                     const std::optional<FunctionCallback> errorCallback = std::nullopt)
            : url(url)
            , responseType(responseType)
            , success(successCallback)
            , error(errorCallback)
        {
        }

      public:
        std::string url;
        std::string responseType;

        // callbacks
        FunctionCallback success;
        std::optional<FunctionCallback> error;
      };

    public:
      RuntimeContext();
      virtual ~RuntimeContext();

    public:
      /**
     * Initialize the fields before using this runtime context.
     */
      virtual void initialize();

      /**
     * Returns if the current thread is in the scripting thread, when the scripting APIs can be safely called.
     */
      bool inScriptingThread() const;

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
     * @param responseCallback The callback to call when the resource is fetched.
     * @param errorCallback The callback to be called when an error occurs during fetching the resource.
     */
      void fetchResource(const std::string &url,
                         const std::string &responseType,
                         const FunctionCallback &responseCallback,
                         const std::optional<FunctionCallback> errorCallback = std::nullopt);

      /**
     * Fetch the resource from the given URL synchronously.
     * 
     * @param url The URL of the resource to fetch.
     * @param responseType The type of the response to expect: "string", "arraybuffer" or "json".
     * @returns The fetched resource.
     */
      v8::Local<v8::Value> fetchResourceSync(const std::string &url, const std::string &responseType);

      /**
     * Fetch the text source type of the resource from the given URL.
     *
     * @param url The URL of the resource to fetch.
     * @param responseCallback The callback to call when the resource is fetched.
     * @param errorCallback The callback to be called when an error occurs during fetching the resource.
     */
      void fetchTextSourceResource(const std::string &url,
                                   const StringResponseCallback &responseCallback,
                                   const std::optional<ErrorCallback> errorCallback = std::nullopt);

      /**
     * Fetch the text source type of the resource from the given URL synchronously.
     * 
     * @param url The URL of the resource to fetch.
     * @param ignoreErrors Whether to ignore errors during fetching the resource.
     * @returns The fetched resource.
     */
      std::string fetchTextSourceResourceSync(const std::string &url, bool ignoreErrors = false);

      /**
     * Fetch the arraybuffer-like resource from the given URL.
     *
     * @param url The URL of the image to fetch.
     * @param responseCallback The callback to call when the image is fetched.
     * @param errorCallback The callback to be called when an error occurs during fetching the resource.
     */
      void fetchArrayBufferLikeResource(const std::string &url,
                                        const BufferResponseCallback &responseCallback,
                                        const std::optional<ErrorCallback> errorCallback = std::nullopt);

      /**
     * Fetch the image resource from the given URL.
     *
     * @param url The URL of the image to fetch.
     * @param responseCallback The callback to call when the image is fetched.
     * @param errorCallback The callback to be called when an error occurs during fetching the resource.
     */
      inline void fetchImageResource(const std::string &url,
                                     const BufferResponseCallback &responseCallback,
                                     const std::optional<ErrorCallback> errorCallback = std::nullopt)
      {
        return fetchArrayBufferLikeResource(url, responseCallback, errorCallback);
      }

      /**
     * Create the WHATWG `fetch` function with the given base URI.
     *
     * @param context The context to create the function in.
     */
      v8::Local<v8::Value> createWHATWGFetchImpl(v8::Local<v8::Context> context);

      /**
     * Send a WHATWG Fetch request, it's used in the `fetch` function implementation.
     * 
     * @param isolate The V8 isolate.
     * @param resource The resource to fetch.
     * @param options The options of the fetch request.
     * @returns The promise of the fetch request.
     */
      v8::Local<v8::Promise> sendWHATWGFetchRequest(v8::Isolate *isolate,
                                                    v8::Local<v8::Value> resource,
                                                    v8::Local<v8::Value> options);

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

    private:
      void fetchResourceImpl(const string &url,
                             const string &responseType,
                             const FunctionCallback &responseCallback,
                             const optional<FunctionCallback> errorCallback);
      void execFetchRequests();

      // Fetch the resource from the given URL, and return a promise.
      v8::Local<v8::Value> callFetchFunction(const std::string &url, const std::string &responseType);

    public:
      std::string baseURI;
      shared_ptr<DOMScriptingContext> scriptingContext;

    protected:
      v8::Isolate *isolate;
      v8::Global<v8::Object> resourceLoaderValue;

    private:
      std::optional<std::thread::id> scripting_thread_ = std::nullopt;
      std::deque<FetchRequest> pending_fetch_requests_;
      std::shared_mutex pending_fetch_requests_mutex_;
      uv_async_t *fetch_async_handle_ = nullptr;
    };
  }
} // namespace endor
