#pragma once

#include <client/dom/comment.hpp>
#include "./character_data-inl.hpp"

namespace dombinding
{
  class Comment : public CharacterDataBase<Comment, dom::Comment>
  {
    using CharacterDataBase::CharacterDataBase;
    friend class NodeBase<Comment, dom::Comment>;

  public:
    static void Init(Napi::Env env);
    static Napi::Value NewInstance(Napi::Env env, std::shared_ptr<dom::Node> nodeImpl);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
