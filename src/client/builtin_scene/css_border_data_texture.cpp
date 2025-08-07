#include "css_border_data_texture.hpp"
#include "instanced_mesh.hpp"
#include <client/macros.h>

namespace builtin_scene
{
  using namespace std;
  using namespace client_graphics;

  CSSBorderDataTexture::CSSBorderDataTexture()
      : glContext_(nullptr)
      , borderDataTexture_(nullptr)
      , currentTextureHeight_(0)
      , isDirty_(true)
  {
  }

  CSSBorderDataTexture::~CSSBorderDataTexture()
  {
    // Texture cleanup is handled by WebGL context
  }

  bool CSSBorderDataTexture::initialize(shared_ptr<WebGL2Context> glContext)
  {
    if (TR_UNLIKELY(!glContext))
      return false;

    glContext_ = glContext;
    borderDataTexture_ = glContext_->createTexture();
    
    if (TR_UNLIKELY(!borderDataTexture_))
      return false;

    // Initialize with minimal size (will be resized as needed)
    currentTextureHeight_ = 1;
    textureData_.resize(5 * 4 * currentTextureHeight_, 0.0f); // 5 columns × 4 components × height

    // Setup texture parameters
    glContext_->activeTexture(WebGLTextureUnit::kTexture1);
    glContext_->bindTexture(WebGLTextureBindingTarget::k2D, borderDataTexture_);
    
    // Use nearest filtering for precise texel fetch
    glContext_->texParameteri(WebGLTextureBindingTarget::k2D, 
                              WebGLTextureParameter::kMinFilter, 
                              WebGLTextureMinificationFilter::kNearest);
    glContext_->texParameteri(WebGLTextureBindingTarget::k2D, 
                              WebGLTextureParameter::kMagFilter, 
                              WebGLTextureMagnificationFilter::kNearest);
    
    // Clamp to edge to avoid sampling issues
    glContext_->texParameteri(WebGLTextureBindingTarget::k2D, 
                              WebGLTextureParameter::kWrapS, 
                              WebGLTextureWrapMode::kClampToEdge);
    glContext_->texParameteri(WebGLTextureBindingTarget::k2D, 
                              WebGLTextureParameter::kWrapT, 
                              WebGLTextureWrapMode::kClampToEdge);

    // Upload initial empty data
    glContext_->texImage2D(WebGLTextureBindingTarget::k2D,
                           0, // mip level
                           WEBGL2_RGBA32F, // internal format - high precision float
                           5, // width (5 columns)
                           currentTextureHeight_, // height
                           0, // border
                           WebGLTextureFormat::kRGBA, // format
                           WebGLPixelType::kFloat, // type
                           textureData_.data()); // data

    return true;
  }

  void CSSBorderDataTexture::updateBorderData(const vector<shared_ptr<Instance>>& instances)
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
      const auto& instance = instances[i];
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
    glContext_->bindTexture(WebGLTextureBindingTarget::k2D, borderDataTexture_);
    glContext_->texSubImage2D(WebGLTextureBindingTarget::k2D,
                              0, // mip level
                              0, 0, // x, y offset
                              5, // width
                              instanceCount, // height
                              WebGLTextureFormat::kRGBA, // format
                              WebGLPixelType::kFloat, // type
                              textureData_.data()); // data

    isDirty_ = false;
  }

  void CSSBorderDataTexture::bind(WebGLTextureUnit textureUnit)
  {
    if (TR_UNLIKELY(!isInitialized()))
      return;

    glContext_->activeTexture(textureUnit);
    glContext_->bindTexture(WebGLTextureBindingTarget::k2D, borderDataTexture_);
  }

  void CSSBorderDataTexture::ensureTextureSize(size_t instanceCount)
  {
    if (instanceCount <= currentTextureHeight_)
      return;

    // Resize texture data buffer
    size_t newHeight = instanceCount;
    textureData_.resize(5 * 4 * newHeight, 0.0f);

    // Reallocate GPU texture
    glContext_->activeTexture(WebGLTextureUnit::kTexture1);
    glContext_->bindTexture(WebGLTextureBindingTarget::k2D, borderDataTexture_);
    glContext_->texImage2D(WebGLTextureBindingTarget::k2D,
                           0, // mip level
                           WEBGL2_RGBA32F, // internal format
                           5, // width
                           newHeight, // height
                           0, // border
                           WebGLTextureFormat::kRGBA, // format
                           WebGLPixelType::kFloat, // type
                           nullptr); // data (will be uploaded via texSubImage2D)

    currentTextureHeight_ = newHeight;
    isDirty_ = true;
  }

  void CSSBorderDataTexture::extractInstanceBorderData(const Instance& instance,
                                                        glm::vec4& borderWidth,
                                                        glm::vec4 borderColors[4])
  {
    // Extract border width from instance
    // Currently the Instance class has borderWidths_ and borderColors_[4] fields
    // but the setter methods are commented out. For now, we'll use default values
    // and set up the infrastructure for when border data is properly populated.
    
    // TODO: Once Instance border data setters are implemented, extract actual data
    // borderWidth = instance.borderWidths_;
    // for (int i = 0; i < 4; ++i) {
    //   borderColors[i] = instance.borderColors_[i];
    // }

    // For now, use default border values
    // This maintains compatibility while the border data extraction is being implemented
    borderWidth = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Default 1px border on all sides
    
    // Default to red border color for testing/debugging
    glm::vec4 defaultColor(1.0f, 0.0f, 0.0f, 1.0f); // Red
    for (int i = 0; i < 4; ++i)
    {
      borderColors[i] = defaultColor;
    }
  }
}