#include <client/macros.h>
#include <functional>

#include "./css_scroll_shadow_texture.hpp"
#include "./instanced_mesh.hpp"

namespace builtin_scene
{
  using namespace std;
  using namespace client_graphics;

  CSSScrollShadowTexture::CSSScrollShadowTexture()
      : glContext_(nullptr)
      , scrollShadowDataTexture_(nullptr)
      , currentTextureHeight_(0)
  {
  }

  CSSScrollShadowTexture::~CSSScrollShadowTexture()
  {
    // Texture cleanup is handled by WebGL context
  }

  bool CSSScrollShadowTexture::initialize(shared_ptr<WebGL2Context> glContext)
  {
    assert(glContext != nullptr && "WebGL2Context must not be null");

    glContext_ = glContext;
    scrollShadowDataTexture_ = glContext_->createTexture();
    assert(scrollShadowDataTexture_ && "Failed to create WebGL texture");

    // Initialize with default size of 100 instances
    currentTextureHeight_ = 100;
    textureData_.resize(3 * 4 * currentTextureHeight_, 0.0f); // 3 columns × 4 components × height

    // Setup texture parameters
    glContext_->activeTexture(WebGLTextureUnit::kTexture2);
    glContext_->bindTexture(WebGLTextureTarget::kTexture2D, scrollShadowDataTexture_);

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
                             3,                      // width (3 columns)
                             currentTextureHeight_); // height
    return true;
  }

  void CSSScrollShadowTexture::updateScrollShadowData(const vector<shared_ptr<Instance>> &instances)
  {
    if (TR_UNLIKELY(!isInitialized()))
      return;

    size_t instanceCount = instances.size();
    if (instanceCount == 0)
      return;

    // Ensure texture is large enough
    ensureTextureSize(instanceCount);

    // Extract scroll shadow data from instances
    for (size_t i = 0; i < instanceCount; ++i)
    {
      const auto &instance = instances[i];
      if (!instance)
        continue;

      glm::vec4 shadowColor;
      float shadowMaxHeight;
      glm::vec2 scrollOffset;
      glm::vec2 contentSize;
      extractInstanceScrollShadowData(*instance, shadowColor, shadowMaxHeight, scrollOffset, contentSize);

      // Store in texture data buffer
      size_t rowOffset = i * 3 * 4; // Each row has 3 columns × 4 components

      // Column 0: scroll shadow color (r, g, b, a)
      textureData_[rowOffset + 0] = shadowColor.r;
      textureData_[rowOffset + 1] = shadowColor.g;
      textureData_[rowOffset + 2] = shadowColor.b;
      textureData_[rowOffset + 3] = shadowColor.a;

      // Column 1: scroll shadow parameters (maxHeight, scrollOffset.x, scrollOffset.y, contentSize.x)
      textureData_[rowOffset + 4] = shadowMaxHeight;
      textureData_[rowOffset + 5] = scrollOffset.x;
      textureData_[rowOffset + 6] = scrollOffset.y;
      textureData_[rowOffset + 7] = contentSize.x;

      // Column 2: content size continuation (contentSize.y, 0, 0, 0)
      textureData_[rowOffset + 8] = contentSize.y;
      textureData_[rowOffset + 9] = 0.0f;
      textureData_[rowOffset + 10] = 0.0f;
      textureData_[rowOffset + 11] = 0.0f;
    }

    // Upload data to GPU
    glContext_->activeTexture(WebGLTextureUnit::kTexture2);
    glContext_->bindTexture(WebGLTextureTarget::kTexture2D, scrollShadowDataTexture_);
    glContext_->texSubImage2D(WebGLTexture2DTarget::kTexture2D,
                              0, // mip level
                              0,
                              0,                                                       // x, y offset
                              3,                                                       // width
                              instanceCount,                                           // height
                              WebGLTextureFormat::kRGBA,                               // format
                              WebGLPixelType::kFloat,                                  // type
                              reinterpret_cast<unsigned char *>(textureData_.data())); // data
  }

  void CSSScrollShadowTexture::bind(WebGLTextureUnit textureUnit)
  {
    if (TR_UNLIKELY(!isInitialized()))
      return;

    glContext_->activeTexture(textureUnit);
    glContext_->bindTexture(WebGLTextureTarget::kTexture2D, scrollShadowDataTexture_);
  }

  void CSSScrollShadowTexture::ensureTextureSize(size_t instanceCount)
  {
    if (instanceCount <= currentTextureHeight_)
      return;

    // Resize texture data buffer
    size_t newHeight = instanceCount;
    textureData_.resize(3 * 4 * newHeight, 0.0f);

    // Create a new texture with the new size since texStorage2D creates immutable storage
    scrollShadowDataTexture_ = glContext_->createTexture();

    // Setup texture parameters
    glContext_->activeTexture(WebGLTextureUnit::kTexture2);
    glContext_->bindTexture(WebGLTextureTarget::kTexture2D, scrollShadowDataTexture_);

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

    // Use texStorage2D to allocate immutable storage
    glContext_->texStorage2D(WebGLTexture2DTarget::kTexture2D,
                             1,              // mip levels
                             WEBGL2_RGBA32F, // internal format - high precision float
                             3,              // width (3 columns)
                             newHeight);     // height

    currentTextureHeight_ = newHeight;
  }

  void CSSScrollShadowTexture::extractInstanceScrollShadowData(const Instance &instance,
                                                               glm::vec4 &shadowColor,
                                                               float &shadowMaxHeight,
                                                               glm::vec2 &scrollOffset,
                                                               glm::vec2 &contentSize) const
  {
    shadowColor = instance.getScrollShadowColor();
    shadowMaxHeight = instance.getScrollShadowMaxHeight();
    scrollOffset = instance.getScrollOffset();
    contentSize = instance.getContentSize();
  }
}