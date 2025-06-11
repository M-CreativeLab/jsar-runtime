#pragma once

#include <iostream>
#include <memory>
#include <assert.h>
#include "../per_process.hpp"

namespace canvas
{
  enum class RenderingContextType
  {
    Unset,
    RenderingContext2D,
    WebGL,
    WebGL2,
    BitmapRenderer,
  };

  template <typename CanvasType>
  class RenderingContextBase
  {
  public:
    RenderingContextBase(RenderingContextType type, std::shared_ptr<CanvasType> canvasRef)
        : contextType(type), canvasRef(canvasRef), clientContext(TrClientContextPerProcess::Get())
    {
      assert(clientContext != nullptr);
      assert(canvasRef != nullptr && "Canvas reference cannot be null");
    }
    virtual ~RenderingContextBase() = default;

  protected:
    // Notify the context's canvas object that the pixels might be changed.
    void notifyCanvasUpdated()
    {
      if (TR_UNLIKELY(canvasRef.expired()))
        return;

      auto canvas = canvasRef.lock();
      assert(canvas != nullptr && "Canvas reference is expired");
      if (canvas->pixels_updated_callback_ != nullptr)
        canvas->pixels_updated_callback_();
    }

  public:
    RenderingContextType contextType;
    std::weak_ptr<CanvasType> canvasRef;

  protected:
    TrClientContextPerProcess *clientContext = nullptr;
  };
}

#include "./rendering_context2d.hpp"
