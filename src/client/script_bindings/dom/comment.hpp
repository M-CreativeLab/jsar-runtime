#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/comment.hpp>
#include "./character_data.hpp"

namespace script_bindings
{
  namespace dom_bindings
  {
    class Comment;
    using CommentBase = scripting_base::ObjectWrap<Comment, dom::Comment, CharacterData>;

    /**
     * Comment wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps dom::Comment objects for use in V8 JavaScript execution contexts.
     * It provides the standard DOM Comment interface, inheriting from CharacterData.
     */
    class Comment : public CommentBase
    {
      using CommentBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "Comment";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::Comment> nativeComment);

    public:
      Comment(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
