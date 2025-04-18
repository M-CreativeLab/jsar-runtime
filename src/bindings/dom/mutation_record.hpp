#pragma once

#include <napi.h>
#include <client/dom/mutation_observer.hpp>

namespace dombinding
{
  class MutationRecord : public Napi::ObjectWrap<MutationRecord>
  {
  public:
    static void Init(Napi::Env env);
    /**
     * Make a new instance of `MutationRecord` from the given mutation record.
     *
     * @param env The N-API environment.
     * @param record The mutation record.
     */
    static Napi::Object MakeFromImpl(Napi::Env env, const dom::MutationRecord &record);

  public:
    MutationRecord(const Napi::CallbackInfo &info);

  private:
    thread_local static Napi::FunctionReference *constructor;
  };
}
