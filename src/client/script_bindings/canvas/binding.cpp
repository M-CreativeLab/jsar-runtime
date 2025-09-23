#include "binding.hpp"
#include "canvas.hpp"
#include "image_data.hpp"
#include "image_bitmap.hpp"
#include "image_source.hpp"
#include "path2d.hpp"
#include "html_rendering_context.hpp"
#include "canvas_rendering_context_2d.hpp"

namespace script_bindings
{
  namespace canvas
  {
    using namespace v8;

    void Initialize(Local<Context> context)
    {
      Isolate *isolate = context->GetIsolate();
      Local<Object> global = context->Global();

      // Initialize Canvas API classes
      Local<Function> canvasConstructor = Canvas::Initialize(isolate);
      Local<Function> offscreenCanvasConstructor = OffscreenCanvas::Initialize(isolate);
      Local<Function> imageDataConstructor = ImageData::Initialize(isolate);
      Local<Function> imageBitmapConstructor = ImageBitmap::Initialize(isolate);
      Local<Function> imageSourceConstructor = ImageSource::Initialize(isolate);
      Local<Function> path2DConstructor = Path2D::Initialize(isolate);
      Local<Function> htmlRenderingContextConstructor = HTMLRenderingContext::Initialize(isolate);

      // Register constructors with global object
      global->Set(context, String::NewFromUtf8(isolate, "Canvas").ToLocalChecked(), canvasConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "OffscreenCanvas").ToLocalChecked(), offscreenCanvasConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "ImageData").ToLocalChecked(), imageDataConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "ImageBitmap").ToLocalChecked(), imageBitmapConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "ImageSource").ToLocalChecked(), imageSourceConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "Path2D").ToLocalChecked(), path2DConstructor).Check();

      // Note: HTMLRenderingContext is typically not exposed directly to global scope
      // It's accessed through Canvas.getContext() methods
    }
  }
}