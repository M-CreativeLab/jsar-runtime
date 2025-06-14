#pragma once

#include <string>
#include <memory>
#include <client/canvas/canvas.hpp>

#include "./html_element.hpp"

namespace dom
{
  using CanvasRenderingContext = canvas::RenderingContextBase<canvas::Canvas>;

  class HTMLCanvasElement : public HTMLElement,
                            public canvas::ImageSource
  {
  public:
    using HTMLElement::HTMLElement;
    HTMLCanvasElement(std::shared_ptr<Document> ownerDocument)
        : HTMLElement("CANVAS", ownerDocument)
        , canvas::ImageSource()
    {
    }

  public:
    void createdCallback(bool from_scripting) override;
    void connectedCallback() override;
    void attributeChangedCallback(const std::string &name,
                                  const std::string &oldValue,
                                  const std::string &newValue) override;

  public:
    inline size_t width() const override
    {
      return canvas_impl_->width();
    }
    inline size_t height() const override
    {
      return canvas_impl_->height();
    }
    inline bool readPixels(SkPixmap &dst) const override
    {
      return canvas_impl_->readPixels(dst);
    }

    inline geometry::DOMRect getDrawingClientRect() const
    {
      return geometry::DOMRect(0, 0, width(), height());
    }

  public:
    inline void setWidth(size_t width)
    {
      canvas_impl_->setWidth(width);
    }
    inline void setHeight(size_t height)
    {
      canvas_impl_->setHeight(height);
    }

    /**
     * Get the rendering context of the canvas.
     *
     * @param contextTypeStr The type of the rendering context: 2d, webgl, webgl2.
     * @returns The rendering context.
     */
    std::shared_ptr<CanvasRenderingContext> getContext(const std::string &contextTypeStr);

    /**
     * Get the data URL of the canvas.
     *
     * @param type The type of the data URL: image/png, image/jpeg, image/webp.
     * @param encoderOptions The quality of the image.
     * @returns The data URL.
     */
    inline std::string toDataURL(const std::string &type, double encoderOptions)
    {
      return canvas_impl_->toDataURL(type, encoderOptions);
    }

    /**
     * Get the canvas implementation object.
     *
     * @returns The canvas implementation object.
     */
    inline std::shared_ptr<canvas::Canvas> canvasImpl()
    {
      return canvas_impl_;
    }

  private:
    std::shared_ptr<canvas::Canvas> canvas_impl_;
  };
}
