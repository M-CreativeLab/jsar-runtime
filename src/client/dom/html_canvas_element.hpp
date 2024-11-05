#pragma once

#include <string>
#include <memory>
#include "./html_element.hpp"
#include "../canvas/canvas.hpp"

namespace dom
{
  using CanvasRenderingContext = canvas::RenderingContextBase<canvas::Canvas>;

  class HTMLCanvasElement : public HTMLElement,
                            public canvas::ImageSource
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLCanvasElement(weak_ptr<Document> ownerDocument)
        : HTMLElement("CANVAS", ownerDocument),
          canvas::ImageSource(),
          canvasImpl_(std::make_shared<canvas::Canvas>())
    {
    }

  public:
    size_t width() override { return canvasImpl_->width(); }
    size_t height() override { return canvasImpl_->height(); }
    bool readPixels(SkPixmap &dst) override { return canvasImpl_->readPixels(dst); }

  public:
    /**
     * The width of the canvas.
     *
     * @returns The width of the canvas.
     */
    size_t getWidth() { return canvasImpl_->width(); }

    /**
     * Set the width of the canvas, it will resize the canvas.
     *
     * @param width The width of the canvas.
     */
    void setWidth(size_t width)
    {
      canvasImpl_->setWidth(width);
    }

    /**
     * The height of the canvas.
     *
     * @returns The height of the canvas.
     */
    size_t getHeight() { return canvasImpl_->height(); }

    /**
     * Set the height of the canvas, it will resize the canvas.
     *
     * @param height The height of the canvas.
     */
    void setHeight(size_t height)
    {
      canvasImpl_->setHeight(height);
    }

    /**
     * Get the rendering context of the canvas.
     *
     * @param contextTypeStr The type of the rendering context: 2d, webgl, webgl2.
     * @returns The rendering context.
     */
    shared_ptr<CanvasRenderingContext> getContext(const std::string &contextTypeStr)
    {
      canvas::RenderingContextType contextType = canvas::RenderingContextType::Unset;
      if (contextTypeStr == "2d")
        contextType = canvas::RenderingContextType::RenderingContext2D;
      else if (contextTypeStr == "webgl")
        contextType = canvas::RenderingContextType::WebGL;
      else if (contextTypeStr == "webgl2")
        contextType = canvas::RenderingContextType::WebGL2;
      else if (contextTypeStr == "bitmaprenderer")
        contextType = canvas::RenderingContextType::BitmapRenderer;
      else
        return nullptr;

      return canvasImpl_->getContext(contextType);
    }

    /**
     * Get the data URL of the canvas.
     *
     * @param type The type of the data URL: image/png, image/jpeg, image/webp.
     * @param encoderOptions The quality of the image.
     * @returns The data URL.
     */
    std::string toDataURL(const std::string &type, double encoderOptions)
    {
      return canvasImpl_->toDataURL(type, encoderOptions);
    }

    /**
     * Get the canvas implementation object.
     *
     * @returns The canvas implementation object.
     */
    inline std::shared_ptr<canvas::Canvas> canvasImpl() { return canvasImpl_; }

  private:
    std::shared_ptr<canvas::Canvas> canvasImpl_;
  };
}
