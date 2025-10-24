#include "./OVR_multiview2.hpp"

using namespace std;
using namespace v8;

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings::extensions
    {
      void OVR_multiview2::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
        HandleScope scope(isolate);
        Local<ObjectTemplate> instance = tpl->InstanceTemplate();

        instance->Set(isolate, "FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR", v8::Integer::New(isolate, 0x9630));
        instance->Set(isolate, "FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR", v8::Integer::New(isolate, 0x9632));
        instance->Set(isolate, "MAX_VIEWS_OVR", v8::Integer::New(isolate, 0x9631));
        instance->Set(isolate, "FRAMEBUFFER_INCOMPLETE_VIEW_TARGETS_OVR", v8::Integer::New(isolate, 0x9633));

        InstanceMethod(isolate,
                       instance,
                       "framebufferTextureMultiviewOVR",
                       &OVR_multiview2::FramebufferTextureMultiviewOVR);
      }

      void OVR_multiview2::FramebufferTextureMultiviewOVR(const FunctionCallbackInfo<Value> &args)
      {
        // Nothing to do in mixed rendering mode
        cerr << "Warning: you are trying to use OVR_multiview2.framebufferTextureMultiviewOVR()"
                " in mixed rendering mode, which is not supported."
             << endl;
        args.GetReturnValue().SetUndefined();
      }
    }
  }
} // namespace endor
