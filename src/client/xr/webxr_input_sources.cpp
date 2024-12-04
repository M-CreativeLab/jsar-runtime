#include "./common.hpp"
#include "./webxr_input_sources.hpp"
#include "./webxr_session.hpp"

namespace client_xr
{
  XRInputSource::XRInputSource(std::shared_ptr<XRSession> session, xr::TrXRInputSource *inputSourceData)
      : session_(session),
        inputSourceData_(inputSourceData)
  {
  }

  XRInputSourceArray::XRInputSourceArray(std::shared_ptr<XRSession> session)
      : session_(session),
        device_(session->device())
  {
  }
}
