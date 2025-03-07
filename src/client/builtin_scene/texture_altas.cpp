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

      // Allocate the texture storage.
      // glContext->texStorage3D(WebGLTexture3DTarget::kTexture2DArray,
      //                         1,
      //                         WEBGL2_RGBA8,
      //                         width,
      //                         height,
      //                         kMaxLayerCount);

      // Fill the texture with the default color.
      vector<unsigned char> pixels(width * height * kMaxLayerCount * sizeof(unsigned char) * 4, 255 / 3);
      glContext->texImage3D(WebGLTexture3DTarget::kTexture2DArray,
                            0,
                            WEBGL2_RGBA8,
                            width,
                            height,
                            kMaxLayerCount,
                            0,
                            WebGLTextureFormat::kRGBA,
                            WebGLPixelType::kUnsignedByte,
                            pixels.data());
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
      if (width > this->width_)
        width = this->width_;
      if (height > this->height_)
        height = this->height_;
    }
    return handle_->addTexture(width, height);
  }

  std::shared_ptr<Texture> TextureAtlas::resizeTexture(std::shared_ptr<Texture> texture, int width, int height, bool autoDownscale)
  {
    assert(texture != nullptr);
    if (texture->width == width && texture->height == height)
      return texture;

    removeTexture(*texture);
    return addTexture(width, height, autoDownscale);
  }

  void TextureAtlas::removeTexture(const Texture &texture)
  {
    handle_->removeTexture(texture);
  }

  void TextureAtlas::updateTexture(const Texture &texture, const unsigned char *pixels)
  {
    auto glContext = glContext_.lock();
    assert(glContext != nullptr);
    glContext->bindTexture(WebGLTextureTarget::kTexture2DArray, glTexture_);
    // Update the texture with the new pixels or the default values.
    glContext->texSubImage3D(WebGLTexture3DTarget::kTexture2DArray, 0,
                             texture.x, texture.y, texture.layer,
                             texture.width, texture.height, 1,
                             WebGLTextureFormat::kRGBA, WebGLPixelType::kUnsignedByte,
                             const_cast<unsigned char *>(pixels));
    glContext->generateMipmap(WebGLTextureTarget::kTexture2DArray);
    // {
    //   glContext->texParameteri(WebGLTextureTarget::kTexture2D,
    //                            WebGLTextureParameterName::kTextureMinFilter, WEBGL_LINEAR_MIPMAP_LINEAR);
    //   glContext->texParameteri(WebGLTextureTarget::kTexture2D,
    //                            WebGLTextureParameterName::kTextureMagFilter, WEBGL_LINEAR);
    // }
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
