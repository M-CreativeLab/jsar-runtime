#pragma once

#include <memory>
#include "./common.hpp"
#include "../graphics/webgl_context.hpp"

namespace client_xr
{
  class XRLayer
  {
  public:
    XRLayer(std::shared_ptr<XRSession> session);
    virtual ~XRLayer() = default;

  public:
    std::shared_ptr<XRSession> session() const { return session_; }

  protected:
    std::shared_ptr<XRSession> session_;
  };

  class XRWebGLLayer : public XRLayer, public xr::WebGLLayer
  {
  public:
    /**
     * It creates a new `XRWebGLLayer` object.
     * 
     * @param session The XR session.
     * @param glContext The WebGL context to use.
     * @returns The created `XRWebGLLayer` object.
     */
    static std::shared_ptr<XRWebGLLayer> Make(std::shared_ptr<XRSession> session,
                                              std::shared_ptr<client_graphics::WebGLContext> glContext)
    {
      return std::make_shared<XRWebGLLayer>(session, glContext);
    }

  public:
    XRWebGLLayer(std::shared_ptr<XRSession> session, std::shared_ptr<client_graphics::WebGLContext> glContext);
    XRWebGLLayer(xr::WebGLLayer &layerData);

  private:
    std::shared_ptr<client_graphics::WebGLContext> glContext_;
  };

  class XRCompositionLayer : public XRLayer
  {
  public:
    XRCompositionLayer(std::shared_ptr<XRSession> session);

  public:
    bool blendTextureSourceAlpha = false;
    int mipLevels = 1;
    bool needsRedraw = false;
  };
}
