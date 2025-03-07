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
      : ColorMaterial(),
        textureAtlas_(nullptr)
  {
  }

  bool WebContentInstancedMaterial::initialize(shared_ptr<WebGL2Context> glContext,
                                               shared_ptr<WebGLProgram> program)
  {
    if (TR_UNLIKELY(!ColorMaterial::initialize(glContext, program)))
      return false;

    auto texAtlasLoc = glContext->getUniformLocation(program, "instanceTexAltas");
    assert(texAtlasLoc.has_value() && "The instanceTexAltas uniform location is not found.");
    glContext->uniform1i(texAtlasLoc.value(), 0);

    assert(textureAtlas_ == nullptr);
    textureAtlas_ = make_unique<TextureAtlas>(glContext, client_graphics::WebGLTextureUnit::kTexture0);
    assert(textureAtlas_ != nullptr);

    // auto textureLoc = glContext->getUniformLocation(program, "tex");
    // if (TR_UNLIKELY(!textureLoc.has_value()))
    // {
    //   cerr << name() << ": The tex uniform location is not found." << endl;
    //   return false;
    // }
    // glContext->uniform1i(textureLoc.value(), 0);

    // auto textureTransformLoc = glContext->getUniformLocation(program, "textureTransformation");
    // if (TR_UNLIKELY(!textureTransformLoc.has_value()))
    // {
    //   cerr << name() << ": The textureTransformation uniform location is not found." << endl;
    //   return false;
    // }

    // // Set the texture to be flipped by the Y-axis.
    // //
    // // WebGL uses the bottom-left corner as the origin, while Skia or Web uses the top-left, so flip the texture by
    // // the Y-axis to make it consistent.
    // flipTextureByY(true);

    // // Create the texture and configure parameters.
    // texture_ = glContext->createTexture();
    // glContext->activeTexture(WebGLTextureUnit::kTexture0);
    // glContext->bindTexture(WebGLTextureTarget::kTexture2D, texture_);
    // glContext->texImage2D(WebGLTexture2DTarget::kTexture2D, 0, WEBGL2_RGBA8, width_, height_, 0,
    //                       WebGLTextureFormat::kRGBA, WebGLPixelType::kUnsignedByte, nullptr);
    // glContext->texParameteri(
    //     WebGLTextureTarget::kTexture2D, WebGLTextureParameterName::kTextureMinFilter, WEBGL_LINEAR);
    // glContext->texParameteri(
    //     WebGLTextureTarget::kTexture2D, WebGLTextureParameterName::kTextureMagFilter, WEBGL_LINEAR);
    // glContext->texParameteri(
    //     WebGLTextureTarget::kTexture2D, WebGLTextureParameterName::kTextureWrapS, WEBGL_CLAMP_TO_EDGE);
    // glContext->texParameteri(
    //     WebGLTextureTarget::kTexture2D, WebGLTextureParameterName::kTextureWrapT, WEBGL_CLAMP_TO_EDGE);
    // glContext->bindTexture(WebGLTextureTarget::kTexture2D, nullptr);
    return true;
  }

  void WebContentInstancedMaterial::onBeforeDrawMesh(shared_ptr<WebGLProgram> program, shared_ptr<Mesh3d> mesh)
  {
    auto glContext = glContext_.lock();
    assert(glContext != nullptr);
    {
      // if (isOpaque())
      // {
      //   glContext->disable(WEBGL_BLEND);
      //   glContext->depthMask(true);
      // }
      // else
      // {
      //   glContext->enable(WEBGL_BLEND);
      //   glContext->blendFunc(WEBGL_SRC_ALPHA, WEBGL_ONE_MINUS_SRC_ALPHA);
      //   glContext->depthMask(false);
      // }

      assert(textureAtlas_ != nullptr);
      textureAtlas_->onBeforeDraw();
    }
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
        // isOpaque_ = pixmap.computeIsOpaque();

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
    textureAtlas_->updateTexture(*textureRect, pixels);

    // Update an non-empty texture means the texture is updated successfully.
    return pixels != nullptr;
  }
} // namespace builtin_scene::materials
