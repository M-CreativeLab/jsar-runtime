#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/comment.hpp>
#include "./character_data.hpp"

namespace script_bindings
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
    /**
     * The name of the Comment class for V8.
     */
    static std::string Name()
    {
      return "Comment";
    }

    /**
     * Configure the V8 function template with Comment properties and methods.
     */
    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    /**
     * Create a new V8 Comment instance from a native dom::Comment.
     */
    static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::Comment> nativeComment);

    /**
     * Initialize the Comment class and register it with V8.
     */
    static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

  public:
    Comment(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
  };
}