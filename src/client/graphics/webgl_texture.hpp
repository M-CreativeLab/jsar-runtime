#pragma once

#include <map>
#include "common/command_buffers/webgl_constants.hpp"
#include "common/command_buffers/details/program.hpp"
#include "./webgl_object.hpp"

namespace client_graphics
{
  enum class WebGLTextureUnit
  {
    kTexture0 = WEBGL_TEXTURE0,
    kTexture1 = WEBGL_TEXTURE1,
    kTexture2 = WEBGL_TEXTURE2,
    kTexture3 = WEBGL_TEXTURE3,
    kTexture4 = WEBGL_TEXTURE4,
    kTexture5 = WEBGL_TEXTURE5,
    kTexture6 = WEBGL_TEXTURE6,
    kTexture7 = WEBGL_TEXTURE7,
    kTexture8 = WEBGL_TEXTURE8,
    kTexture9 = WEBGL_TEXTURE9,
    kTexture10 = WEBGL_TEXTURE10,
    kTexture11 = WEBGL_TEXTURE11,
    kTexture12 = WEBGL_TEXTURE12,
    kTexture13 = WEBGL_TEXTURE13,
    kTexture14 = WEBGL_TEXTURE14,
    kTexture15 = WEBGL_TEXTURE15,
    kTexture16 = WEBGL_TEXTURE16,
    kTexture17 = WEBGL_TEXTURE17,
    kTexture18 = WEBGL_TEXTURE18,
    kTexture19 = WEBGL_TEXTURE19,
    kTexture20 = WEBGL_TEXTURE20,
    kTexture21 = WEBGL_TEXTURE21,
    kTexture22 = WEBGL_TEXTURE22,
    kTexture23 = WEBGL_TEXTURE23,
    kTexture24 = WEBGL_TEXTURE24,
    kTexture25 = WEBGL_TEXTURE25,
    kTexture26 = WEBGL_TEXTURE26,
    kTexture27 = WEBGL_TEXTURE27,
    kTexture28 = WEBGL_TEXTURE28,
    kTexture29 = WEBGL_TEXTURE29,
    kTexture30 = WEBGL_TEXTURE30,
    kTexture31 = WEBGL_TEXTURE31,
  };

  enum class WebGLTextureTarget
  {
    kTexture2D = WEBGL_TEXTURE_2D,
    kTextureCubeMap = WEBGL_TEXTURE_CUBE_MAP,
    kTexture3D = WEBGL2_TEXTURE_3D,
    kTexture2DArray = WEBGL2_TEXTURE_2D_ARRAY,
  };

  enum class WebGLTexture2DTarget
  {
    kTexture2D = WEBGL_TEXTURE_2D,
    kTextureCubeMapPositiveX = WEBGL_TEXTURE_CUBE_MAP_POSITIVE_X,
    kTextureCubeMapNegativeX = WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    kTextureCubeMapPositiveY = WEBGL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    kTextureCubeMapNegativeY = WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    kTextureCubeMapPositiveZ = WEBGL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    kTextureCubeMapNegativeZ = WEBGL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
  };

  enum class WebGLTexture3DTarget
  {
    kTexture3D = WEBGL2_TEXTURE_3D,
    kTexture2DArray = WEBGL2_TEXTURE_2D_ARRAY,
  };

  enum class WebGLTextureFormat
  {
    kRed = WEBGL2_RED,
    kRedInteger = WEBGL2_RED_INTEGER,
    kRG = WEBGL2_RG,
    kRGInteger = WEBGL2_RG_INTEGER,
    kRGB = WEBGL_RGB,
    kRGBInteger = WEBGL2_RGB_INTEGER,
    kRGBA = WEBGL_RGBA,
    kRGBAInteger = WEBGL2_RGBA_INTEGER,
    kDepthComponent = WEBGL_DEPTH_COMPONENT,
    kDepthStencil = WEBGL_DEPTH_STENCIL,
    kLuminance = WEBGL_LUMINANCE,
    kLuminanceAlpha = WEBGL_LUMINANCE_ALPHA,
    kAlpha = WEBGL_ALPHA,
  };

  enum class WebGLPixelType
  {
    kUnsignedByte = WEBGL_UNSIGNED_BYTE,
    kByte = WEBGL_BYTE,
    kUnsignedShort = WEBGL_UNSIGNED_SHORT,
    kShort = WEBGL_SHORT,
    kUnsignedInt = WEBGL_UNSIGNED_INT,
    kInt = WEBGL_INT,
    kHalfFloat = WEBGL2_HALF_FLOAT,
    kFloat = WEBGL_FLOAT,
    kUnsignedShort565 = WEBGL_UNSIGNED_SHORT_5_6_5,
    kUnsignedShort4444 = WEBGL_UNSIGNED_SHORT_4_4_4_4,
    kUnsignedShort5551 = WEBGL_UNSIGNED_SHORT_5_5_5_1,
    kUnsignedInt2101010Rev = WEBGL2_UNSIGNED_INT_2_10_10_10_REV,
    kUnsignedInt10f11f11fRev = WEBGL2_UNSIGNED_INT_10F_11F_11F_REV,
    kUnsignedInt5999Rev = WEBGL2_UNSIGNED_INT_5_9_9_9_REV,
    kUnsignedInt248 = WEBGL2_UNSIGNED_INT_24_8,
    kFloat32UnsignedInt248Rev = WEBGL2_FLOAT_32_UNSIGNED_INT_24_8_REV,
  };

  enum class WebGLTextureParameterName
  {
    kTextureBaseLevel = WEBGL2_TEXTURE_BASE_LEVEL,
    kTextureCompareFunc = WEBGL2_TEXTURE_COMPARE_FUNC,
    kTextureCompareMode = WEBGL2_TEXTURE_COMPARE_MODE,
    kTextureMinFilter = WEBGL_TEXTURE_MIN_FILTER,
    kTextureMagFilter = WEBGL_TEXTURE_MAG_FILTER,
    kTextureMinLOD = WEBGL2_TEXTURE_MIN_LOD,
    kTextureMaxLOD = WEBGL2_TEXTURE_MAX_LOD,
    kTextureMaxLevel = WEBGL2_TEXTURE_MAX_LEVEL,
    // kTextureSwizzleR = WEBGL_TEXTURE_SWIZZLE_R,
    // kTextureSwizzleG = WEBGL_TEXTURE_SWIZZLE_G,
    // kTextureSwizzleB = WEBGL_TEXTURE_SWIZZLE_B,
    // kTextureSwizzleA = WEBGL_TEXTURE_SWIZZLE_A,
    kTextureWrapS = WEBGL_TEXTURE_WRAP_S,
    kTextureWrapT = WEBGL_TEXTURE_WRAP_T,
    kTextureWrapR = WEBGL2_TEXTURE_WRAP_R,

    // Extensions
    kTextureMaxAnisotropyEXT = WEBGL2_TEXTURE_MAX_ANISOTROPY_EXT,
  };

  class WebGLTexture : public WebGLObject
  {
  public:
    WebGLTexture() : WebGLObject(WebGLObjectType::Texture) {}
  };
}
