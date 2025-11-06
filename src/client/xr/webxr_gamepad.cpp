#include "./webxr_gamepad.hpp"
#include <sstream>

using namespace std;

namespace endor
{
  namespace client_xr
  {
    Gamepad::Gamepad(const xr::TrXRInputSource *inputSource)
        : index_(-1)
        , connected_(true)
        , timestamp_(0)
        , mapping_("xr-standard")
    {
      // Generate ID based on input source name
      stringstream ss;
      ss << "xr-input-" << inputSource->id;
      id_ = ss.str();

      update(inputSource, 0);
    }

    void Gamepad::update(const xr::TrXRInputSource *inputSource, int64_t timestamp)
    {
      timestamp_ = timestamp;
      connected_ = inputSource->enabled;

      // Update axes
      axes_.clear();
      for (int i = 0; i < inputSource->axesCount && i < xr::TrXRInputSource::MaxAxes; i++)
      {
        axes_.push_back(inputSource->axes[i]);
      }

      // Update buttons
      buttons_.clear();
      for (int i = 0; i < inputSource->buttonsCount && i < xr::TrXRInputSource::MaxButtons; i++)
      {
        const auto &btn = inputSource->buttons[i];
        buttons_.emplace_back(btn.pressed, btn.touched, btn.value);
      }
    }
  }
} // namespace endor
