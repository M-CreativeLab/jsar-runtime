#include <client/macros.h>
#include <functional>

#include "./css_border_data_texture.hpp"
#include "./instanced_mesh.hpp"

namespace builtin_scene
{
  using namespace std;
  using namespace client_graphics;

  CSSBorderDataTexture::CSSBorderDataTexture()
      : glContext_(nullptr)
      , borderDataTexture_(nullptr)
      , currentTextureHeight_(0)
  {
  }

  CSSBorderDataTexture::~CSSBorderDataTexture()
  {
    // Texture cleanup is handled by WebGL context
  }

  bool CSSBorderDataTexture::initialize(shared_ptr<WebGL2Context> glContext)
  {
    assert(glContext != nullptr && "WebGL2Context must not be null");

    glContext_ = glContext;
    borderDataTexture_ = glContext_->createTexture();
    assert(borderDataTexture_ && "Failed to create WebGL texture");

    // Initialize with default size of 100 instances
    currentTextureHeight_ = 100;
    textureData_.resize(5 * 4 * currentTextureHeight_, 0.0f); // 5 columns × 4 components × height

    // Setup texture parameters
    glContext_->activeTexture(WebGLTextureUnit::kTexture1);
    glContext_->bindTexture(WebGLTextureTarget::kTexture2D, borderDataTexture_);

    // Use nearest filtering for precise texel fetch
    glContext_->texParameteri(WebGLTextureTarget::kTexture2D,
                              WebGLTextureParameterName::kTextureMinFilter,
                              WEBGL_NEAREST);
    glContext_->texParameteri(WebGLTextureTarget::kTexture2D,
                              WebGLTextureParameterName::kTextureMagFilter,
                              WEBGL_NEAREST);

    // Clamp to edge to avoid sampling issues
    glContext_->texParameteri(WebGLTextureTarget::kTexture2D,
                              WebGLTextureParameterName::kTextureWrapS,
                              WEBGL_CLAMP_TO_EDGE);
    glContext_->texParameteri(WebGLTextureTarget::kTexture2D,
                              WebGLTextureParameterName::kTextureWrapT,
                              WEBGL_CLAMP_TO_EDGE);

    // Use texStorage2D to allocate immutable storage
    glContext_->texStorage2D(WebGLTexture2DTarget::kTexture2D,
                             1,                      // mip levels
                             WEBGL2_RGBA32F,         // internal format - high precision float
                             5,                      // width (5 columns)
                             currentTextureHeight_); // height
    return true;
  }

  void CSSBorderDataTexture::updateBorderData(const vector<shared_ptr<Instance>> &instances)
  {
    if (TR_UNLIKELY(!isInitialized()))
      return;

    size_t instanceCount = instances.size();
    if (instanceCount == 0)
      return;

    // Ensure texture is large enough
    ensureTextureSize(instanceCount);

    // Extract border data from instances
    for (size_t i = 0; i < instanceCount; ++i)
    {
      const auto &instance = instances[i];
      if (!instance)
        continue;

      glm::vec4 borderWidth;
      glm::vec4 borderColors[4];
      extractInstanceBorderData(*instance, borderWidth, borderColors);

      // Store in texture data buffer
      size_t rowOffset = i * 5 * 4; // Each row has 5 columns × 4 components

      // Column 0: border widths (top, right, bottom, left)
      textureData_[rowOffset + 0] = borderWidth.x; // top
      textureData_[rowOffset + 1] = borderWidth.y; // right
      textureData_[rowOffset + 2] = borderWidth.z; // bottom
      textureData_[rowOffset + 3] = borderWidth.w; // left

      // Columns 1-4: border colors for each side
      for (int side = 0; side < 4; ++side)
      {
        size_t colorOffset = rowOffset + (side + 1) * 4;
        textureData_[colorOffset + 0] = borderColors[side].r;
        textureData_[colorOffset + 1] = borderColors[side].g;
        textureData_[colorOffset + 2] = borderColors[side].b;
        textureData_[colorOffset + 3] = borderColors[side].a;
      }
    }

    // Upload data to GPU
    glContext_->activeTexture(WebGLTextureUnit::kTexture1);
    glContext_->bindTexture(WebGLTextureTarget::kTexture2D, borderDataTexture_);
    glContext_->texSubImage2D(WebGLTexture2DTarget::kTexture2D,
                              0, // mip level
                              0,
                              0,                                                       // x, y offset
                              5,                                                       // width
                              instanceCount,                                           // height
                              WebGLTextureFormat::kRGBA,                               // format
                              WebGLPixelType::kFloat,                                  // type
                              reinterpret_cast<unsigned char *>(textureData_.data())); // data
  }

  void CSSBorderDataTexture::bind(WebGLTextureUnit textureUnit)
  {
    if (TR_UNLIKELY(!isInitialized()))
      return;

    glContext_->activeTexture(textureUnit);
    glContext_->bindTexture(WebGLTextureTarget::kTexture2D, borderDataTexture_);
  }

  void CSSBorderDataTexture::ensureTextureSize(size_t instanceCount)
  {
    if (instanceCount <= currentTextureHeight_)
      return;

    // Resize texture data buffer
    size_t newHeight = instanceCount;
    textureData_.resize(5 * 4 * newHeight, 0.0f);

    // Create a new texture with the new size since texStorage2D creates immutable storage
    borderDataTexture_ = glContext_->createTexture();

    // Setup texture parameters
    glContext_->activeTexture(WebGLTextureUnit::kTexture1);
    glContext_->bindTexture(WebGLTextureTarget::kTexture2D, borderDataTexture_);

    glContext_->texParameteri(WebGLTextureTarget::kTexture2D,
                              WebGLTextureParameterName::kTextureMinFilter,
                              WEBGL_NEAREST);
    glContext_->texParameteri(WebGLTextureTarget::kTexture2D,
                              WebGLTextureParameterName::kTextureMagFilter,
                              WEBGL_NEAREST);
    glContext_->texParameteri(WebGLTextureTarget::kTexture2D,
                              WebGLTextureParameterName::kTextureWrapS,
                              WEBGL_CLAMP_TO_EDGE);
    glContext_->texParameteri(WebGLTextureTarget::kTexture2D,
                              WebGLTextureParameterName::kTextureWrapT,
                              WEBGL_CLAMP_TO_EDGE);

    // Allocate new immutable texture storage
    glContext_->texStorage2D(WebGLTexture2DTarget::kTexture2D,
                             1,              // mip levels
                             WEBGL2_RGBA32F, // internal format
                             5,              // width
                             newHeight);     // height

    currentTextureHeight_ = newHeight;
  }

  void CSSBorderDataTexture::extractInstanceBorderData(const Instance &instance,
                                                       glm::vec4 &borderWidth,
                                                       glm::vec4 borderColors[4])
  {
    // Extract border width and colors from instance
    borderWidth = instance.getBorderWidths();

    const glm::vec4 *instanceBorderColors = instance.getBorderColors();
    borderColors[0] = instanceBorderColors[0];
    borderColors[1] = instanceBorderColors[1];
    borderColors[2] = instanceBorderColors[2];
    borderColors[3] = instanceBorderColors[3];
  }
}
