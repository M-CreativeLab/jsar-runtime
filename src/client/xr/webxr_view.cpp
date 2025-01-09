#include "./webxr_view.hpp"
#include "./webxr_spaces.hpp"
#include "./webxr_rigid_transform.hpp"
#include "./webxr_session.hpp"
#include "./device.hpp"

namespace client_xr
{
  XRView::XRView(xr::TrXRView &viewData,
                 std::shared_ptr<XRSession> session,
                 std::shared_ptr<XRReferenceSpace> baseReferenceSpace)
      : index_(viewData.viewIndex),
        sessionId_(session ? session->id : throw std::invalid_argument("Session cannot be null")),
        projectionMatrix_(viewData.getProjectionMatrix())
  {
    if (!baseReferenceSpace)
      throw std::invalid_argument("Base reference space cannot be null");

    auto viewport = session->device()->getViewport(index_);
    viewport_ = XRViewport(viewport);

    auto viewBaseMatrix = baseReferenceSpace->inverseBaseMatrix() * glm::inverse(viewData.getViewMatrix());
    transform_ = XRRigidTransform(viewBaseMatrix);
  }
} // namespace client_xr
