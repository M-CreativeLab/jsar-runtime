#include "./texture_altas.hpp"

namespace builtin_scene
{
  using namespace std;
  using namespace glm;
  using namespace client_graphics;

  TextureAtlas::TextureAtlas(shared_ptr<WebGL2Context> glContext, WebGLTextureUnit unit, int width, int height)
      : glContext_(glContext),
        glTexture_(glContext->createTexture()),
        width_(width),
        height_(height),
        unit_(unit),
        handle_(make_unique<crates::texture_atlas::TextureAtlasLayout>(width, height, kMaxLayerCount))
  {
    assert(glContext != nullptr);
    assert(unit_ >= WebGLTextureUnit::kTexture0 &&
           unit_ <= WebGLTextureUnit::kTexture31);

    glContext->activeTexture(unit_);
    glContext->bindTexture(WebGLTextureTarget::kTexture2DArray, glTexture_);
    {
      glContext->texParameteri(WebGLTextureTarget::kTexture2DArray,
                               WebGLTextureParameterName::kTextureMinFilter,
                               WEBGL_LINEAR_MIPMAP_LINEAR);
      glContext->texParameteri(WebGLTextureTarget::kTexture2DArray,
                               WebGLTextureParameterName::kTextureMagFilter,
                               WEBGL_LINEAR);
      glContext->texParameteri(WebGLTextureTarget::kTexture2DArray,
                               WebGLTextureParameterName::kTextureWrapS,
                               WEBGL_CLAMP_TO_EDGE);
      glContext->texParameteri(WebGLTextureTarget::kTexture2DArray,
                               WebGLTextureParameterName::kTextureWrapT,
                               WEBGL_CLAMP_TO_EDGE);

      if (glContext->supportsExtension("EXT_texture_filter_anisotropic"))
      {
        glContext->texParameterf(WebGLTextureTarget::kTexture2DArray,
                                 WebGLTextureParameterName::kTextureMaxAnisotropyEXT,
                                 glContext->maxTextureMaxAnisotropy);
      }

      // Initialize the texture atlas with the default values.
      glContext->texImage3D(WebGLTexture3DTarget::kTexture2DArray,
                            0,
                            WEBGL2_RGBA8,
                            width,
                            height,
                            kMaxLayerCount,
                            0,
                            WebGLTextureFormat::kRGBA,
                            WebGLPixelType::kUnsignedByte,
                            nullptr);
      glContext->generateMipmap(WebGLTextureTarget::kTexture2DArray);
    }
    glContext->bindTexture(WebGLTextureTarget::kTexture2DArray, nullptr);
  }

  TextureAtlas::~TextureAtlas()
  {
    auto glContext = glContext_.lock();
    if (glContext != nullptr)
      glContext->deleteTexture(glTexture_);
  }

  shared_ptr<Texture> TextureAtlas::addTexture(int width, int height, bool autoDownscale)
  {
    if (autoDownscale)
    {
      const float widthRatio = static_cast<float>(width_) / width;
      const float heightRatio = static_cast<float>(height_) / height;
      const float scale = fmin(widthRatio, heightRatio);
      if (scale < 1.0f)
      {
        int scaledWidth = static_cast<int>(floor(width * scale));
        int scaledHeight = static_cast<int>(floor(height * scale));

        scaledWidth = scaledWidth > width_ ? width_ : scaledWidth;
        scaledHeight = scaledHeight > height_ ? height_ : scaledHeight;

        if (scaledWidth <= 0 || scaledHeight <= 0)
          throw runtime_error("Invalid texture size");

        width = scaledWidth;
        height = scaledHeight;
      }
    }

    if (width > width_ || height > height_)
      throw runtime_error("Invalid texture size");
    return handle_->addTexture(width, height);
  }

  std::shared_ptr<Texture> TextureAtlas::resizeTexture(std::shared_ptr<Texture> texture, int width, int height, bool autoDownscale)
  {
    assert(texture != nullptr);
    if (texture->width == width &&
        texture->height == height)
      return texture;

    removeTexture(*texture);
    return addTexture(width, height, autoDownscale);
  }

  void TextureAtlas::removeTexture(const Texture &texture)
  {
    handle_->removeTexture(texture);
  }

  void TextureAtlas::updateTexture(const Texture &texture, const unsigned char *pixels,
                                   WebGLTextureFormat format,
                                   WebGLPixelType pixelType)
  {
    auto glContext = glContext_.lock();
    assert(glContext != nullptr);

    glContext->bindTexture(WebGLTextureTarget::kTexture2DArray, glTexture_);
    // Update the texture with the new pixels or the default values.
    glContext->texSubImage3D(WebGLTexture3DTarget::kTexture2DArray, 0,
                             texture.x, texture.y, texture.layer,
                             texture.width, texture.height, 1,
                             format, pixelType,
                             const_cast<unsigned char *>(pixels));

    glContext->generateMipmap(WebGLTextureTarget::kTexture2DArray);
    glContext->bindTexture(WebGLTextureTarget::kTexture2DArray, nullptr);
  }

  void TextureAtlas::onBeforeDraw()
  {
    auto glContext = glContext_.lock();
    assert(glContext != nullptr);
    glContext->activeTexture(unit_);
    glContext->bindTexture(WebGLTextureTarget::kTexture2DArray, glTexture_);
  }

  void TextureAtlas::onAfterDraw()
  {
    auto glContext = glContext_.lock();
    assert(glContext != nullptr);
    glContext->bindTexture(WebGLTextureTarget::kTexture2DArray, nullptr);
  }
}
