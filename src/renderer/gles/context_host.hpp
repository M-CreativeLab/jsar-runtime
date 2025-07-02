#pragma once

#include <optional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "./context_storage.hpp"
#include "./framebuffer.hpp"
#include "./framebuffer_attachment.hpp"

/**
 * The `ContextGLStorage` for the host such as: Unity, Unreal Engine, etc.
 * 
 * It records the host's context states and fbos, and provides methods to restore them.
 */
class ContextGLHost : public ContextGLStorage
{
public:
  ContextGLHost();

public:
  void recordFromHost();
  void recordTextureBindingFromHost();
  void restore();

  void configureFramebuffer();
  void restoreFramebuffer();

  inline const GLuint currentFramebufferId() const
  {
    return m_FramebufferId.value_or(0);
  }
  inline bool isCurrentFramebufferValid() const
  {
    GLuint id = currentFramebufferId();
    return id > 0 && host_framebuffers_.find(id) != host_framebuffers_.end();
  }
  inline std::optional<const GLFramebuffer> currentFramebuffer() const
  {
    GLuint id = currentFramebufferId();
    if (host_framebuffers_.find(id) == host_framebuffers_.end())
      return std::nullopt;
    else
      return *host_framebuffers_.at(id);
  }
  inline GLFramebuffer &currentFramebufferChecked() const
  {
    return *host_framebuffers_.at(currentFramebufferId());
  }
  inline bool isHostFramebufferColorOnly() const
  {
    return currentFramebufferChecked().isColorAttachmentOnly();
  }

  // Create a render target(framebuffer) for application to render into.
  std::shared_ptr<GLRenderTarget> createRenderTarget();

private:
  void onHostFramebufferChanged();

private:
  std::unordered_map<GLuint, std::shared_ptr<GLFramebuffer>> host_framebuffers_;
};
