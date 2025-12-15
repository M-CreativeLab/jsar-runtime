#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <command_buffers/gpu/gpu_texture.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  void TrContextWebGL::glActiveTexture(WebGLenum unit)
  {
    active_texture_unit_ = unit;
  }

  void TrContextWebGL::glBindTexture(WebGLenum target, WebGLuint id)
  {
    auto texture = textures_.get(id);
    if (!texture) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }

    auto texture_target = details::TextureTarget(target);
    texture->setTarget(texture_target);
    texture_bindings_[texture_target] = texture;
  }

  void TrContextWebGL::glCopyTexImage2D(WebGLenum target,
                                        WebGLenum internalformat,
                                        WebGLint level,
                                        WebGLint x,
                                        WebGLint y,
                                        WebGLsizei width,
                                        WebGLsizei height,
                                        WebGLint border)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    texture->mipLevels = level + 1;
    texture->internalformat = internalformat;
    texture->setSize(width, height);
  }

  void TrContextWebGL::glCopyTexSubImage2D(WebGLenum target,
                                           WebGLint level,
                                           WebGLint xoffset,
                                           WebGLint yoffset,
                                           WebGLint x,
                                           WebGLint y,
                                           WebGLsizei width,
                                           WebGLsizei height)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
  }

  void TrContextWebGL::glDeleteTextures(WebGLsizei n, const WebGLuint *textures)
  {
    for (WebGLsizei i = 0; i < n; i++)
    {
      for (auto it = textures_.begin(); it != textures_.end(); ++it)
      {
        Ref<details::Texture> texture = *it;
        if (texture->id == textures[i])
        {
          textures_.erase(it);
          break;
        }
      }
    }
  }

  void TrContextWebGL::glGenTextures(WebGLsizei n, WebGLuint *textures)
  {
    glGenTypedObjects(textures_, n, textures);
  }

  void TrContextWebGL::glGetTexParameter(WebGLenum target, WebGLenum pname, WebGLint *params)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture || !params) [[unlikely]]
      return;
    auto it = texture->params_i.find(pname);
    if (it != texture->params_i.end())
      *params = it->second;
  }

  WebGLboolean TrContextWebGL::glIsTexture(WebGLuint texture)
  {
    return textures_.has(texture);
  }

  void TrContextWebGL::glTexImage2D(WebGLenum target,
                                    WebGLint level,
                                    WebGLenum internalformat,
                                    WebGLsizei width,
                                    WebGLsizei height,
                                    WebGLsizei border,
                                    WebGLenum format,
                                    WebGLenum type,
                                    const WebGLvoid *data)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    texture->mipLevels = std::max(1, level + 1);
    texture->internalformat = internalformat;
    texture->setSize(width, height);

    auto rr = getRenderResource();
    GPUTextureDescriptor desc = {};
    desc.label = "WebGLTexture";
    desc.usage = GPUTextureUsage::kTextureBinding;
    desc.dimension = commandbuffers::GPUTextureDimension::k2D;
    desc.size = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1u };
    desc.mipLevelCount = static_cast<uint32_t>(texture->mipLevels);
    desc.format = GPUTextureFormat::kRGBA8Unorm;
    auto gpu_tex = rr->createTexture(&desc);
    if (gpu_tex != nullptr)
      texture->gpu_id = gpu_tex->id;
  }

  void TrContextWebGL::glTexImage3D(WebGLenum target,
                                    WebGLint level,
                                    WebGLenum internalformat,
                                    WebGLsizei width,
                                    WebGLsizei height,
                                    WebGLsizei depth,
                                    WebGLsizei border,
                                    WebGLenum format,
                                    WebGLenum type,
                                    const WebGLvoid *data)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    texture->mipLevels = std::max(1, level + 1);
    texture->internalformat = internalformat;
    texture->setSize(width, height, depth);

    auto rr = getRenderResource();
    GPUTextureDescriptor desc = {};
    desc.label = "WebGLTexture";
    desc.usage = GPUTextureUsage::kTextureBinding;
    if (target == WEBGL2_TEXTURE_3D)
    {
      desc.dimension = commandbuffers::GPUTextureDimension::k3D;
      desc.size = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), static_cast<uint32_t>(depth) };
    }
    else
    {
      desc.dimension = commandbuffers::GPUTextureDimension::k2D;
      desc.size = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), static_cast<uint32_t>(depth) };
    }
    desc.mipLevelCount = static_cast<uint32_t>(texture->mipLevels);
    desc.format = GPUTextureFormat::kRGBA8Unorm;
    auto gpu_tex = rr->createTexture(&desc);
    if (gpu_tex != nullptr)
      texture->gpu_id = gpu_tex->id;
  }

  // --- Additional texture APIs ---
  void TrContextWebGL::glCompressedTexImage2D(WebGLenum target,
                                              WebGLint level,
                                              WebGLenum internalformat,
                                              WebGLsizei width,
                                              WebGLsizei height,
                                              WebGLsizei border,
                                              WebGLsizei imageSize,
                                              const WebGLvoid *data)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    texture->mipLevels = std::max(1, level + 1);
    texture->internalformat = internalformat;
    texture->setSize(width, height);
  }

  void TrContextWebGL::glCompressedTexImage3D(WebGLenum target,
                                              WebGLint level,
                                              WebGLenum internalformat,
                                              WebGLsizei width,
                                              WebGLsizei height,
                                              WebGLsizei depth,
                                              WebGLsizei border,
                                              WebGLsizei imageSize,
                                              const WebGLvoid *data)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    texture->mipLevels = std::max(1, level + 1);
    texture->internalformat = internalformat;
    texture->setSize(width, height, depth);
  }

  void TrContextWebGL::glCompressedTexSubImage2D(WebGLenum target,
                                                 WebGLint level,
                                                 WebGLint xoffset,
                                                 WebGLint yoffset,
                                                 WebGLsizei width,
                                                 WebGLsizei height,
                                                 WebGLenum format,
                                                 WebGLsizei imageSize,
                                                 const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glCompressedTexSubImage3D(WebGLenum target,
                                                 WebGLint level,
                                                 WebGLint xoffset,
                                                 WebGLint yoffset,
                                                 WebGLint zoffset,
                                                 WebGLsizei width,
                                                 WebGLsizei height,
                                                 WebGLsizei depth,
                                                 WebGLenum format,
                                                 WebGLsizei imageSize,
                                                 const WebGLvoid *data)
  {
    // TODO(yorkie): implement
  }

  void TrContextWebGL::glTexParameterf(WebGLenum target, WebGLenum pname, WebGLfloat param)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture) [[unlikely]]
      return;
    texture->params_f[pname] = param;
  }

  void TrContextWebGL::glTexParameteri(WebGLenum target, WebGLenum pname, WebGLint param)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture) [[unlikely]]
      return;
    texture->params_i[pname] = param;
  }

  void TrContextWebGL::glTexParameterfv(WebGLenum target, WebGLenum pname, const WebGLfloat *params)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture || !params) [[unlikely]]
      return;
    texture->params_f[pname] = *params;
  }

  void TrContextWebGL::glTexParameteriv(WebGLenum target, WebGLenum pname, const WebGLint *params)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture || !params) [[unlikely]]
      return;
    texture->params_i[pname] = *params;
  }

  void TrContextWebGL::glTexStorage2D(WebGLenum target,
                                      WebGLint levels,
                                      WebGLenum internalformat,
                                      WebGLsizei width,
                                      WebGLsizei height)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    texture->mipLevels = std::max(1, levels);
    texture->internalformat = internalformat;
    texture->setSize(width, height);
    auto rr = getRenderResource();
    GPUTextureDescriptor desc = {};
    desc.label = "WebGLTexture";
    desc.usage = GPUTextureUsage::kTextureBinding;
    desc.dimension = commandbuffers::GPUTextureDimension::k2D;
    desc.size = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1u };
    desc.mipLevelCount = static_cast<uint32_t>(texture->mipLevels);
    desc.format = GPUTextureFormat::kRGBA8Unorm;
    auto gpu_tex = rr->createTexture(&desc);
    if (gpu_tex != nullptr)
      texture->gpu_id = gpu_tex->id;
  }

  void TrContextWebGL::glTexStorage3D(WebGLenum target,
                                      WebGLint levels,
                                      WebGLenum internalformat,
                                      WebGLsizei width,
                                      WebGLsizei height,
                                      WebGLsizei depth)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
    texture->mipLevels = std::max(1, levels);
    texture->internalformat = internalformat;
    texture->setSize(width, height, depth);
    auto rr = getRenderResource();
    GPUTextureDescriptor desc = {};
    desc.label = "WebGLTexture";
    desc.usage = GPUTextureUsage::kTextureBinding;
    if (target == WEBGL2_TEXTURE_3D)
    {
      desc.dimension = commandbuffers::GPUTextureDimension::k3D;
      desc.size = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), static_cast<uint32_t>(depth) };
    }
    else
    {
      desc.dimension = commandbuffers::GPUTextureDimension::k2D;
      desc.size = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), static_cast<uint32_t>(depth) };
    }
    desc.mipLevelCount = static_cast<uint32_t>(texture->mipLevels);
    desc.format = GPUTextureFormat::kRGBA8Unorm;
    auto gpu_tex = rr->createTexture(&desc);
    if (gpu_tex != nullptr)
      texture->gpu_id = gpu_tex->id;
  }

  void TrContextWebGL::glTexSubImage2D(WebGLenum target,
                                       WebGLint level,
                                       WebGLint xoffset,
                                       WebGLint yoffset,
                                       WebGLsizei width,
                                       WebGLsizei height,
                                       WebGLenum format,
                                       WebGLenum type,
                                       const WebGLvoid *data)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
  }

  void TrContextWebGL::glTexSubImage3D(WebGLenum target,
                                       WebGLint level,
                                       WebGLint xoffset,
                                       WebGLint yoffset,
                                       WebGLint zoffset,
                                       WebGLsizei width,
                                       WebGLsizei height,
                                       WebGLsizei depth,
                                       WebGLenum format,
                                       WebGLenum type,
                                       const WebGLvoid *data)
  {
    auto texture = texture_bindings_.at(target);
    if (!texture) [[unlikely]]
    {
      last_error_ = WEBGL_INVALID_OPERATION;
      return;
    }
  }
}
