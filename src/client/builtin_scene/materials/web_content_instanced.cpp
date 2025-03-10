#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkImageInfo.h>
#include <skia/include/core/SkPixmap.h>
#include <skia/include/core/SkData.h>
#include <skia/include/core/SkStream.h>
#include <skia/include/encode/SkPngEncoder.h>
#include <glm/glm.hpp>
#include <client/macros.h>

#include "./web_content_instanced.hpp"

namespace builtin_scene::materials
{
  using namespace std;
  using namespace skia::textlayout;
  using namespace client_graphics;

  WebContentInstancedMaterial::WebContentInstancedMaterial()
      : Material(),
        width_(0.0f),
        height_(0.0f),
        textureAtlas_(nullptr),
        textureOffset_(0.0f, 0.0f),
        textureScale_(1.0f, 1.0f)
  {
    this->isOpaque_ = true;
  }

  bool WebContentInstancedMaterial::initialize(shared_ptr<WebGL2Context> glContext,
                                               shared_ptr<WebGLProgram> program)
  {
    if (TR_UNLIKELY(!Material::initialize(glContext, program)))
      return false;

#define LOAD_UNIFORM_LOCATION(name)                                               \
  {                                                                               \
    auto loc = glContext->getUniformLocation(program, name);                      \
    assert(loc.has_value() && "The \"" name "\" uniform location is not found."); \
    uniforms_.emplace(name, loc.value());                                         \
  }

    LOAD_UNIFORM_LOCATION("instanceTexAltas");
    LOAD_UNIFORM_LOCATION("textureTransformation");
#undef LOAD_UNIFORM_LOCATION
    glContext->uniform1f(uniform("instanceTexAltas"), 0);

    // Set the texture to be flipped by the Y-axis.
    //
    // WebGL uses the bottom-left corner as the origin, while Skia or Web uses the top-left, so flip the texture by
    // the Y-axis to make it consistent.
    flipTextureByY(true);

    // Initialize the texture atlas.
    assert(textureAtlas_ == nullptr && "The texture atlas is already initialized.");
    textureAtlas_ = make_unique<TextureAtlas>(glContext, client_graphics::WebGLTextureUnit::kTexture0);
    return textureAtlas_ != nullptr; // Tells the caller whether the initialization is successful.
  }

  void WebContentInstancedMaterial::onBeforeDrawMesh(shared_ptr<WebGLProgram> program, shared_ptr<Mesh3d> mesh)
  {
    auto glContext = glContext_.lock();
    assert(glContext != nullptr);

    // Update the uniforms
    glContext->uniform1i(uniform("instanceTexAltas"), 0);
    glContext->uniformMatrix3fv(uniform("textureTransformation"), false,
                                glm::mat3(
                                    textureScale_.x, 0.0f, 0.0f,
                                    0.0f, textureScale_.y, 0.0f,
                                    textureOffset_.x, textureOffset_.y, 1.0f));

    // Bind the texture atlas.
    assert(textureAtlas_ != nullptr);
    textureAtlas_->onBeforeDraw();
  }

  void WebContentInstancedMaterial::onAfterDrawMesh(shared_ptr<WebGLProgram> program, shared_ptr<Mesh3d> mesh)
  {
    textureAtlas_->onAfterDraw();
  }

  void WebContentInstancedMaterial::flipTextureByY(bool flip)
  {
    if (flip)
    {
      textureOffset_ = glm::vec2(0.0f, 1.0f);
      textureScale_ = glm::vec2(1.0f, -1.0f);
    }
    else
    {
      textureOffset_ = glm::vec2(0.0f, 0.0f);
      textureScale_ = glm::vec2(1.0f, 1.0f);
    }
  }

  bool WebContentInstancedMaterial::updateTexture(WebContent &content)
  {
    if (textureAtlas_ == nullptr)
      return false; // Just skip the update when the texture atlas is not ready.

    auto textureRect = content.resizeOrInitTexture(*textureAtlas_);
    if (textureRect == nullptr)
      return false; // Just skip when the texture creation is failed.

    unsigned char *pixels = nullptr;
    int internalformat = WEBGL2_RGBA8;
    WebGLTextureFormat format = WebGLTextureFormat::kRGBA;
    WebGLPixelType pixelType = WebGLPixelType::kUnsignedByte;

    SkCanvas *canvas = content.canvas();
    SkSurface *surface = canvas->getSurface();
    if (surface != nullptr)
    {
      SkImageInfo info = surface->imageInfo();
      SkPixmap pixmap;
      if (surface->peekPixels(&pixmap))
      {
        pixels = (unsigned char *)pixmap.addr();
        content.setOpaque(pixmap.computeIsOpaque());

        // Update the texture format based on the Skia surface color type.
        SkColorType colorType = surface->imageInfo().colorType();
        switch (colorType)
        {
        case kRGBA_8888_SkColorType:
          // Keep the default values.
          break;
        case kRGB_888x_SkColorType:
          format = WebGLTextureFormat::kRGB;
          internalformat = WEBGL2_RGB8;
          break;
        case kRGBA_F16_SkColorType:
          pixelType = WebGLPixelType::kHalfFloat;
          internalformat = WEBGL2_RGBA16F;
          break;
        case kRGBA_F32_SkColorType:
          pixelType = WebGLPixelType::kFloat;
          internalformat = WEBGL2_RGBA32F;
          break;
        case kBGRA_8888_SkColorType:
          cerr << name() << ": The BGRA_8888 color type is not supported." << endl;
          break;
        default:
          cerr << name() << ": The color type is not supported." << endl;
          break;
        };
      }
      else
      {
        cerr << name() << ": The pixels are not readable." << endl;
      }
    }

    // Update the texture with the new pixels or the default values.
    textureAtlas_->updateTexture(*textureRect, pixels, format, pixelType);

    // Update an non-empty texture means the texture is updated successfully.
    return pixels != nullptr;
  }
} // namespace builtin_scene::materials
