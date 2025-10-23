#pragma once

#include <cassert>
#include <memory>
#include <node/v8.h>
#include <client/canvas/image_source.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    /**
     * Get a underlying canvas::ImageSource from a V8 JavaScript value.
     * 
     * This function checks if the provided V8 value is an instance of any known ImageSource types:
     * ImageBitmap, ImageData, OffscreenCanvas, HTMLCanvasElement, or HTMLImageElement.
     * 
     * If a match is found, it unwraps the corresponding C++ object and returns its underlying canvas::ImageSource. If 
     * the value is not an object or does not match any known types, it returns nullptr.
     * 
     * @param isolate The V8 isolate.
     * @param value The V8 value to check.
     * @return A shared pointer to the underlying canvas::ImageSource, or nullptr if not found.
     */
    std::shared_ptr<canvas::ImageSource> GetImageSourceFromValue(v8::Isolate *isolate, v8::Local<v8::Value> value);
  }
}
