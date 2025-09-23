#include "./comment.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  void Comment::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    // Comment inherits all functionality from CharacterData
    // No additional properties or methods specific to Comment
  }

  Local<Object> Comment::NewInstance(Isolate *isolate, shared_ptr<dom::Comment> nativeComment)
  {
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Function> constructor = Comment::GetConstructorFunction(isolate);
    Local<Object> instance = constructor->NewInstance(context, 0, nullptr).ToLocalChecked();

    Comment *wrapper = new Comment(isolate, *reinterpret_cast<const FunctionCallbackInfo<Value> *>(&instance));
    wrapper->SetNativeInstance(nativeComment);
    Comment::Wrap(isolate, instance, wrapper);

    return instance;
  }

  Local<Function> Comment::Initialize(Isolate *isolate)
  {
    return Comment::ObjectWrap::Initialize(isolate);
  }

  Comment::Comment(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
    : CommentBase(isolate, args)
  {
    // Comment constructor - creates a new comment node
    // TODO: Handle constructor arguments (initial data)
  }
}