#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkImageInfo.h>
#include <skia/include/core/SkPixmap.h>
#include <skia/include/core/SkData.h>
#include <skia/include/core/SkStream.h>
#include <skia/include/encode/SkPngEncoder.h>
#include <glm/glm.hpp>
#include <client/macros.h>
#include "./web_content.hpp"

namespace builtin_scene::materials
{
  using namespace std;
  using namespace skia::textlayout;
  using namespace client_graphics;

  bool WebContentMaterial::initialize(shared_ptr<WebGL2Context> glContext,
                                      shared_ptr<WebGLProgram> program)
  {
    using namespace client_graphics;

    if (TR_UNLIKELY(!ColorMaterial::initialize(glContext, program)))
      return false;

    auto textureLoc = glContext->getUniformLocation(program, "tex");
    if (TR_UNLIKELY(!textureLoc.has_value()))
    {
      cerr << name() << ": The tex uniform location is not found." << endl;
      return false;
    }
    glContext->uniform1i(textureLoc.value(), 0);

    auto textureTransformLoc = glContext->getUniformLocation(program, "textureTransformation");
    if (TR_UNLIKELY(!textureTransformLoc.has_value()))
    {
      cerr << name() << ": The textureTransformation uniform location is not found." << endl;
      return false;
    }

    texture_ = glContext->createTexture();
    glContext->activeTexture(WebGLTextureUnit::kTexture0);
    glContext->bindTexture(WebGLTextureTarget::kTexture2D, texture_);
    glContext->texImage2D(WebGLTexture2DTarget::kTexture2D, 0, WEBGL2_RGBA8, width_, height_, 0,
                          WebGLTextureFormat::kRGBA, WebGLPixelType::kUnsignedByte, nullptr);
    glContext->texParameteri(
        WebGLTextureTarget::kTexture2D, WebGLTextureParameterName::kTextureMinFilter, WEBGL_LINEAR);
    glContext->texParameteri(
        WebGLTextureTarget::kTexture2D, WebGLTextureParameterName::kTextureMagFilter, WEBGL_LINEAR);
    glContext->texParameteri(
        WebGLTextureTarget::kTexture2D, WebGLTextureParameterName::kTextureWrapS, WEBGL_CLAMP_TO_EDGE);
    glContext->texParameteri(
        WebGLTextureTarget::kTexture2D, WebGLTextureParameterName::kTextureWrapT, WEBGL_CLAMP_TO_EDGE);
    glContext->bindTexture(WebGLTextureTarget::kTexture2D, nullptr);
    return true;
  }

  void WebContentMaterial::onBeforeDrawMesh(shared_ptr<WebGLProgram> program, shared_ptr<Mesh3d> mesh)
  {
    auto glContext = glContext_.lock();
    assert(glContext != nullptr);
    {
      // Update the blend mode.
      glContext->enable(WEBGL_BLEND);
      glContext->blendFunc(WEBGL_SRC_ALPHA, WEBGL_ONE_MINUS_SRC_ALPHA);

      glContext->activeTexture(WebGLTextureUnit::kTexture0);
      glContext->bindTexture(WebGLTextureTarget::kTexture2D, texture_);

      auto textureLoc = glContext->getUniformLocation(program, "tex");
      assert(textureLoc.has_value());
      glContext->uniform1i(textureLoc.value(), 0);

      float tx = 0.0f;
      float ty = 0.0f;
      float sx = 1.0f;
      float sy = 1.0f;
      glm::mat3 textureTransform = glm::mat3(
          sx, 0.0f, 0.0f,
          0.0f, sy, 0.0f,
          tx, ty, 1.0f);
      glContext->uniformMatrix3fv(glContext->getUniformLocation(program, "textureTransformation").value(),
                                  false,
                                  textureTransform);
    }
  }

  void WebContentMaterial::onAfterDrawMesh(shared_ptr<WebGLProgram> program, shared_ptr<Mesh3d> mesh)
  {
    auto glContext = glContext_.lock();
    assert(glContext != nullptr);
    glContext->bindTexture(WebGLTextureTarget::kTexture2D, nullptr);
  }

  void WebContentMaterial::updateTexture(WebContent &content)
  {
    width_ = content.width();
    height_ = content.height();

    if (texture_ != nullptr) // If the texture_ has been initialized, update it.
    {
      auto glContext = glContext_.lock();
      assert(glContext != nullptr);
      glContext->bindTexture(WebGLTextureTarget::kTexture2D, texture_);

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
#ifdef TR_CLIENT_WEB_CONTENT_DEBUG
          {
            static bool needsWrite = true;
            if (needsWrite)
            {
              SkFILEWStream fs(".WEBCONTENTS_DEBUG.png");
              SkPngEncoder::Encode(&fs, pixmap, SkPngEncoder::Options());
              needsWrite = false;
            }
          }
#endif

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
      // cout << "Updating texture with width: " << width_ << ", height: " << height_ << endl;
      // cout << " pixels: " << (void *)pixels << endl;
      glContext->texImage2D(WebGLTexture2DTarget::kTexture2D, 0, internalformat, width_, height_, 0,
                            format, pixelType, pixels);
      glContext->generateMipmap(WebGLTextureTarget::kTexture2D);
      {
        glContext->texParameteri(WebGLTextureTarget::kTexture2D,
                                 WebGLTextureParameterName::kTextureMinFilter, WEBGL_LINEAR_MIPMAP_LINEAR);
        glContext->texParameteri(WebGLTextureTarget::kTexture2D,
                                 WebGLTextureParameterName::kTextureMagFilter, WEBGL_LINEAR);
      }
      glContext->bindTexture(WebGLTextureTarget::kTexture2D, nullptr);
    }
  }
} // namespace builtin_scene::materials
