#include <client/script_bindings/fetch/global_fetch.hpp>
#include <client/script_bindings/fetch/response.hpp>
#include <client/dom/browsing_context.hpp>
#include <client/fileapi/blob.hpp>
#include <client/url/url.hpp>

using namespace std;
using namespace v8;

namespace script_bindings
{
  // static
  Local<Value> GlobalFetch::Fetch(shared_ptr<dom::Document> document, const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    EscapableHandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Value> resourceValue = args[0];
    Local<Value> optionsValue = Undefined(isolate);
    Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();

    if (args.Length() >= 2)
    {
      optionsValue = args[1];
    }

    if (resourceValue->IsString())
    {
      // Check if the string starts with "blob:"
      String::Utf8Value utf8Value(isolate, resourceValue);
      string resourceStr = string(*utf8Value);

      if (resourceStr.rfind("blob:", 0) == 0)
      {
        // It's a blob URL, resolve it to a Blob object
        shared_ptr<client_fileapi::Blob> blob = client_url::URL::BlobRegistry.get(resourceStr);
        resolver->Resolve(context, Response::NewInstance(isolate, blob)).ToChecked();
        return scope.Escape(resolver->GetPromise());
      }
    }

    auto browsingContext = document->browsingContext;
    auto responsePromise = browsingContext->sendWHATWGFetchRequest(isolate, resourceValue, optionsValue);

    auto OnResolve = [](const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      Local<Promise::Resolver> resolver = args.Data().As<Promise::Resolver>();
      resolver->Resolve(context, Response::NewInstance(isolate, args[0])).ToChecked();
    };
    auto OnReject = [](const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      Local<Promise::Resolver> resolver = args.Data().As<Promise::Resolver>();
      Local<Value> error = args[0];
      resolver->Reject(context, error).ToChecked();
    };

    // Link the fetch promise to the returned resolver
    responsePromise->Then(context,
                          Function::New(context, OnResolve, resolver).ToLocalChecked(),
                          Function::New(context, OnReject, resolver).ToLocalChecked())
      .ToLocalChecked();

    // Return the resolver promise
    return scope.Escape(resolver->GetPromise());
  }

  // static
  Local<Value> GlobalFetch::FetchLater(shared_ptr<dom::Document> document, const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    return Local<Value>();
  }
}
