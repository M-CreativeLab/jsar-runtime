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

  bool XRInputSource::dispatchSelectOrSqueezeEvents(XRFrame &frame)
  {
    assert(session_ != nullptr);

    if (inputSourceData_->primaryActionPressed == true)
    {
      // When the primary action is pressed.
      if (primaryActionPressed_ == false)
      {
        // When the primary action is pressed for the first time.
        primaryActionPressed_ = true;
        session_->onPrimaryActionStart(*this, frame);
      }
    }
    else
    {
      // When the primary action is not pressed.
      if (primaryActionPressed_ == true)
      {
        // When the primary action is released.
        primaryActionPressed_ = false;
        session_->onPrimaryActionEnd(*this, frame);
      }
    }

    if (inputSourceData_->squeezeActionPressed == true)
    {
      // When the squeeze action is pressed.
      if (squeezeActionPressed_ == false)
      {
        // When the squeeze action is pressed for the first time.
        squeezeActionPressed_ = true;
        session_->onSqueezeActionStart(*this, frame);
      }
    }
    else
    {
      // When the squeeze action is not pressed.
      if (squeezeActionPressed_ == true)
      {
        // When the squeeze action is released.
        squeezeActionPressed_ = false;
        session_->onSqueezeActionEnd(*this, frame);
      }
    }
    return true;
  }

  XRInputSourceArray::XRInputSourceArray(std::shared_ptr<XRSession> session)
      : session_(session),
        device_(session->device())
  {
  }
}
