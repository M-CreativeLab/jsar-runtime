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

  public:
    RenderingContextType contextType;
    std::weak_ptr<CanvasType> canvasRef;

  protected:
    TrClientContextPerProcess *clientContext = nullptr;
  };
}

#include "./rendering_context2d.hpp"
