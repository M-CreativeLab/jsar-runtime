#pragma once

#include <functional>
#include <optional>
#include <memory>
#include <string>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>

#include "./rendering_context.hpp"
#include "./image_source.hpp"

namespace canvas
{
  /**
   * @class CanvasBase
   * The `CanvasBase` class is a base class for canvas implementations, providing common functionality for both `Canvas` and `OffscreenCanvas`.
   *
   * @tparam T The derived class type (e.g., `Canvas` or `OffscreenCanvas`).
   */
  template <typename T>
  class CanvasBase : public ImageSource,
                     public std::enable_shared_from_this<T>
  {
  public:
    friend class canvas::RenderingContextBase<T>;
    friend class canvas::CanvasRenderingContext2D<T>;

    static constexpr const int DEFAULT_CANVAS_WIDTH = 300;
    static constexpr const int DEFAULT_CANVAS_HEIGHT = 150;

  public:
    /**
     * Default constructor for `CanvasBase`.
     */
    CanvasBase(std::optional<int> width = std::nullopt,
               std::optional<int> height = std::nullopt)
        : ImageSource(),
          widthToSet(width.has_value() ? static_cast<uint32_t>(width.value()) : DEFAULT_CANVAS_WIDTH),
          heightToSet(height.has_value() ? static_cast<uint32_t>(height.value()) : DEFAULT_CANVAS_HEIGHT),
          bitmap_(std::make_shared<SkBitmap>())
    {
      resetSkSurface();
    }

  public:
    /**
     * Creates or gets the rendering context with the specified type.
     *
     * @param type The type of the rendering context: 2d, webgl, webgl2.
     * @returns The rendering context.
     * @throws std::invalid_argument if the context type is unsupported.
     */
    std::shared_ptr<RenderingContextBase<T>> getContext(RenderingContextType type);

  public:
    /**
     * Gets the width of the canvas.
     *
     * @returns The width of the canvas in pixels.
     */
    size_t width() const override final
    {
      return skSurface == nullptr ? 0 : skSurface->width();
    }

    /**
     * Gets the height of the canvas.
     *
     * @returns The height of the canvas in pixels.
     */
    size_t height() const override final
    {
      return skSurface == nullptr ? 0 : skSurface->height();
    }

    /**
     * Reads the pixel data from the canvas into the specified `SkPixmap`.
     *
     * @param dst The destination `SkPixmap` to store the pixel data.
     * @returns True if the pixel data was successfully read, false otherwise.
     */
    bool readPixels(SkPixmap &dst) const override final
    {
      if (skSurface == nullptr)
        return false;
      return skSurface->peekPixels(&dst);
    }

    /**
     * This callback is used to listen for pixel updates on the canvas.
     */
    void setPixelsUpdatedCallback(std::function<void()> callback)
    {
      pixels_updated_callback_ = std::move(callback);
    }

  public:
    /**
     * Sets the width of the canvas.
     *
     * @param width The new width of the canvas in pixels.
     */
    void setWidth(uint32_t width)
    {
      widthToSet = width;
      resize();
    }

    /**
     * Sets the height of the canvas.
     *
     * @param height The new height of the canvas in pixels.
     */
    void setHeight(uint32_t height)
    {
      heightToSet = height;
      resize();
    }

    /**
     * Resizes the canvas to the specified width and height.
     */
    inline void resize() { resetSkSurface(); }

    /**
     * @returns The constant shared pointer to the SkBitmap that stores the canvas content.
     */
    inline std::shared_ptr<const SkBitmap> getSkBitmap() const { return bitmap_; }

  protected:
    /**
     * Gets a shared pointer to the derived class instance.
     *
     * @returns A shared pointer to the derived class instance.
     */
    std::shared_ptr<T> getPtr() { return this->shared_from_this(); }

    /**
     * Resets the Skia surface to match the current width and height.
     */
    void resetSkSurface();

  protected:
    sk_sp<SkSurface> skSurface; // The Skia surface for rendering
    uint32_t widthToSet;        // The width to set for the canvas
    uint32_t heightToSet;       // The height to set for the canvas

    // Cached rendering contexts
    //
    // A canvas can have only one instance of a type of rendering context, such as there is only a 2d context, but at
    // the same time, developers can get a `2d` context and a `gl` context, both them could draw on this canvas.
    std::shared_ptr<CanvasRenderingContext2D<T>> renderingContext2d;
    // TODO(yorkie): support other types of rendering contexts, such as WebGL, WebGL2, etc.

  private:
    std::shared_ptr<SkBitmap> bitmap_; // The bitmap storage for the canvas content
    std::function<void()> pixels_updated_callback_;
  };

  /**
   * @class Canvas
   * The `Canvas` class represents an HTML canvas element.
   */
  class Canvas final : public CanvasBase<Canvas>
  {
  public:
    using CanvasBase::CanvasBase;

  public:
    /**
     * Converts the canvas content to a data URL.
     *
     * @param type The MIME type of the image (e.g., "image/png").
     * @param encoderOptions The encoding options for the image.
     * @returns A data URL containing the canvas content.
     */
    std::string toDataURL(const std::string &type, double encoderOptions);
  };

  /**
   * @class OffscreenCanvas
   * The `OffscreenCanvas` class represents an offscreen canvas element.
   */
  class OffscreenCanvas final : public CanvasBase<OffscreenCanvas>
  {
  public:
    /**
     * Constructs a new `OffscreenCanvas` with the specified width and height.
     *
     * @param width The width of the canvas in pixels.
     * @param height The height of the canvas in pixels.
     */
    OffscreenCanvas(uint32_t width, uint32_t height);

  public:
    /**
     * Commits the offscreen canvas content to the main thread.
     */
    void commit();
  };
} // namespace canvas

#include "./canvas-inl.hpp"
