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
      : Material(false)
      , width_(0.0f)
      , height_(0.0f)
      , textureAtlas_(nullptr)
      , textureOffset_(0.0f, 0.0f)
      , textureScale_(1.0f, 1.0f)
  {
  }

  bool WebContentInstancedMaterial::initialize(shared_ptr<WebGL2Context> glContext,
                                               shared_ptr<WebGLProgram> program)
  {
    if (TR_UNLIKELY(!Material::initialize(glContext, program)))
      return false;

#define LOAD_UNIFORM_LOCATION(name)                                               \
  {                                                                               \
    auto loc = glContext->getUniformLocation(program, name);                      \
    if (loc.has_value()) {                                                        \
      uniforms_.emplace(name, loc.value());                                       \
    }                                                                            \
  }

    LOAD_UNIFORM_LOCATION("instanceTexAltas");
    LOAD_UNIFORM_LOCATION("textureTransformation");
    LOAD_UNIFORM_LOCATION("uSdfEnabled");
    LOAD_UNIFORM_LOCATION("borderDataTexture");
    // Fallback uniforms (only present when USE_INSTANCE_SDF is not defined)
    LOAD_UNIFORM_LOCATION("uDimensions");
    LOAD_UNIFORM_LOCATION("uBorderRadius");
    LOAD_UNIFORM_LOCATION("uBorderWidth");
    LOAD_UNIFORM_LOCATION("uBorderColor");
    LOAD_UNIFORM_LOCATION("uBorderStyle");
#undef LOAD_UNIFORM_LOCATION
    glContext->uniform1i(uniform("instanceTexAltas"), 0);

    // Set the texture to be flipped by the Y-axis.
    //
    // WebGL uses the bottom-left corner as the origin, while Skia or Web uses the top-left, so flip the texture by
    // the Y-axis to make it consistent.
    flipTextureByY(true);

    // Initialize the texture atlas.
    assert(textureAtlas_ == nullptr && "The texture atlas is already initialized.");
    textureAtlas_ = make_unique<TextureAtlas>(glContext, client_graphics::WebGLTextureUnit::kTexture0);
    
    // Initialize border data texture
    borderDataTexture_ = glContext->createTexture();
    
    return textureAtlas_ != nullptr; // Tells the caller whether the initialization is successful.
  }

  void WebContentInstancedMaterial::onBeforeDrawMesh(shared_ptr<WebGLProgram> program, shared_ptr<Mesh3d> mesh)
  {
    auto glContext = glContext_.lock();
    assert(glContext != nullptr);

    // Update the uniforms
    glContext->uniform1i(uniform("instanceTexAltas"), 0);
    // Set border data texture to texture unit 1
    if (hasUniform("borderDataTexture"))
    {
      glContext->uniform1i(uniform("borderDataTexture"), 1);
    }
    glContext->uniformMatrix3fv(uniform("textureTransformation"),
                                false,
                                glm::mat3(textureScale_.x, 0.0f, 0.0f, 0.0f, textureScale_.y, 0.0f, textureOffset_.x, textureOffset_.y, 1.0f));

    // Update SDF uniforms (instance data is set per instance, not here)
    glContext->uniform1f(uniform("uSdfEnabled"), sdfEnabled_ ? 1.0f : 0.0f);

    // Update border data buffer if this is an instanced mesh
    if (mesh->isInstancedMesh())
    {
      auto instancedMesh = mesh->getHandleAs<InstancedMeshBase>();
      if (instancedMesh != nullptr)
      {
        std::vector<glm::vec4> borderWidths;
        std::vector<glm::vec4> borderColors;
        instancedMesh->getBorderData(borderWidths, borderColors);
        
        if (!borderWidths.empty() && borderDataBuffer_ != nullptr)
        {
          updateBorderData(borderWidths, borderColors);
        }
      }
    }

    // Update border data texture if dirty
    if (borderDataDirty_ && borderDataTexture_ != nullptr && !borderWidths_.empty())
    {
      // Prepare texture data: each row is one instance, columns are [width, topColor, rightColor, bottomColor, leftColor]
      size_t instanceCount = borderWidths_.size();
      std::vector<float> textureData(instanceCount * 5 * 4); // 5 vec4s per instance, 4 floats per vec4
      
      for (size_t i = 0; i < instanceCount; ++i)
      {
        size_t baseIndex = i * 5 * 4;
        
        // Column 0: border widths
        textureData[baseIndex + 0] = borderWidths_[i].x;
        textureData[baseIndex + 1] = borderWidths_[i].y;
        textureData[baseIndex + 2] = borderWidths_[i].z;
        textureData[baseIndex + 3] = borderWidths_[i].w;
        
        // Get border color (replicate to all sides for now)
        const auto& color = (i < borderColors_.size()) ? borderColors_[i] : glm::vec4(0.0f);
        
        // Columns 1-4: border colors (top, right, bottom, left) - for now all the same
        for (int side = 0; side < 4; ++side)
        {
          size_t colorIndex = baseIndex + (side + 1) * 4;
          textureData[colorIndex + 0] = color.r;
          textureData[colorIndex + 1] = color.g;
          textureData[colorIndex + 2] = color.b;
          textureData[colorIndex + 3] = color.a;
        }
      }
      
      // Update texture
      glContext->activeTexture(client_graphics::WebGLTextureUnit::kTexture1);
      glContext->bindTexture(client_graphics::WebGLTextureBindingTarget::k2D, borderDataTexture_);
      
      // Set texture parameters for nearest neighbor sampling (no interpolation needed for data)
      glContext->texParameteri(client_graphics::WebGLTextureBindingTarget::k2D, 
                              client_graphics::WebGLTextureParameter::kMinFilter, 
                              static_cast<int>(client_graphics::WebGLTextureFilter::kNearest));
      glContext->texParameteri(client_graphics::WebGLTextureBindingTarget::k2D, 
                              client_graphics::WebGLTextureParameter::kMagFilter, 
                              static_cast<int>(client_graphics::WebGLTextureFilter::kNearest));
      glContext->texParameteri(client_graphics::WebGLTextureBindingTarget::k2D, 
                              client_graphics::WebGLTextureParameter::kWrapS, 
                              static_cast<int>(client_graphics::WebGLTextureWrap::kClampToEdge));
      glContext->texParameteri(client_graphics::WebGLTextureBindingTarget::k2D, 
                              client_graphics::WebGLTextureParameter::kWrapT, 
                              static_cast<int>(client_graphics::WebGLTextureWrap::kClampToEdge));
      
      // Upload texture data (5 columns x instanceCount rows, RGBA32F format)
      glContext->texImage2D(client_graphics::WebGLTextureBindingTarget::k2D,
                           0, // mip level
                           WEBGL2_RGBA32F, // internal format
                           5, // width (5 vec4s per instance)
                           static_cast<int>(instanceCount), // height
                           0, // border
                           client_graphics::WebGLTextureFormat::kRGBA,
                           client_graphics::WebGLPixelType::kFloat,
                           textureData.data());
      
      borderDataDirty_ = false;
    }

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

  void WebContentInstancedMaterial::setSdfEnabled(bool enabled)
  {
    sdfEnabled_ = enabled;
  }

  void WebContentInstancedMaterial::updateBorderData(const std::vector<glm::vec4>& borderWidths, 
                                                     const std::vector<glm::vec4>& borderColors)
  {
    borderWidths_ = borderWidths;
    borderColors_ = borderColors;
    borderDataDirty_ = true;
  }

  WebContentInstancedMaterial::TextureUpdateStatus WebContentInstancedMaterial::updateTexture(WebContent &content)
  {
    if (textureAtlas_ == nullptr)
      return TextureUpdateStatus::kFailed; // Just skip the update when the texture atlas is not ready.

    auto textureRect = content.resizeOrInitTexture(*textureAtlas_);
    if (textureRect == nullptr)
      return TextureUpdateStatus::kSkipped; // Just skip when the texture creation is failed.

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

    // No matter the texture update is successful or not, we will return the status.
    return TextureUpdateStatus::kSuccess;
  }

} // namespace builtin_scene::materials
