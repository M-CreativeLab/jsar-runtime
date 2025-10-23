#include "image_source.hpp"
#include <client/canvas/image_source.hpp>
#include <client/script_bindings/canvas/image_bitmap.hpp>
#include <client/script_bindings/canvas/image_data.hpp>
#include <client/script_bindings/canvas/canvas.hpp>
#include <client/script_bindings/html/html_canvas_element.hpp>
#include <client/script_bindings/html/html_image_element.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace canvas_bindings
    {
      using namespace std;
      using namespace v8;

      shared_ptr<canvas::ImageSource> GetImageSourceFromValue(Isolate *isolate, Local<Value> value)
      {
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        if (!value->IsObject())
          return nullptr;

        auto object = value->ToObject(context).ToLocalChecked();
        if (ImageBitmap::IsInstanceOf(isolate, object))
        {
          auto imageBitmapWrapper = ImageBitmap::Unwrap(isolate, object);
          if (imageBitmapWrapper)
            return imageBitmapWrapper->handle();
          return nullptr;
        }

        if (ImageData::IsInstanceOf(isolate, object))
        {
          auto imageDataWrapper = ImageData::Unwrap(isolate, object);
          if (imageDataWrapper)
            return imageDataWrapper->handle();
          return nullptr;
        }

        if (OffscreenCanvas::IsInstanceOf(isolate, object))
        {
          auto canvasWrapper = OffscreenCanvas::Unwrap(isolate, object);
          if (canvasWrapper)
            return canvasWrapper->handle();
          return nullptr;
        }

        if (html_bindings::HTMLCanvasElement::IsInstanceOf(isolate, object))
        {
          auto canvasWrapper = html_bindings::HTMLCanvasElement::Unwrap(isolate, object);
          if (canvasWrapper)
            return canvasWrapper->handle();
          return nullptr;
        }

        if (html_bindings::HTMLImageElement::IsInstanceOf(isolate, object))
        {
          auto imageWrapper = html_bindings::HTMLImageElement::Unwrap(isolate, object);
          if (imageWrapper)
            return imageWrapper->handle();
          return nullptr;
        }

        // TODO(yorkie): support other types, e.g. Video, SVG, etc.
        // Check for ImageData
        return nullptr;
      }
    }
  }
} // namespace endor
