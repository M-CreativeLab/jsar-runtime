#include "./session.hpp"
#include "renderer/content_renderer.hpp"

namespace xr
{
  TrXRSession::TrXRSession(uint32_t id, TrContentRenderer *contentRenderer, TrXRSessionInit &init)
      : id(id),
        contentRenderer(contentRenderer),
        init(init)
  {
  }

  TrXRSession::~TrXRSession()
  {
  }

  bool TrXRSession::belongsTo(TrContentRenderer *contentRenderer)
  {
    return this->contentRenderer == contentRenderer;
  }

  bool TrXRSession::belongsTo(pid_t contentPid)
  {
    return contentRenderer->getContentPid() == contentPid;
  }
}
