#include "./comment.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings::dom_bindings
{
  void Comment::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    // Comment inherits all functionality from CharacterData
    // No additional properties or methods specific to Comment
  }

  Local<Object> Comment::NewInstance(Isolate *isolate, shared_ptr<dom::Comment> nativeComment)
  {
    EscapableHandleScope scope(isolate);
    if (nativeComment == nullptr)
    {
      return scope.Escape(Local<Object>());
    }
    else
    {
      return scope.Escape(CommentBase::NewInstance(isolate, nativeComment).As<Object>());
    }
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