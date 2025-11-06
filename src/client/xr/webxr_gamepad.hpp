#pragma once

#include <memory>
#include <vector>
#include <string>
#include <common/xr/input_sources.hpp>

namespace endor
{
  namespace client_xr
  {
    /**
     * Represents the state of a gamepad button.
     * See: https://www.w3.org/TR/gamepad/#gamepadbutton-interface
     */
    class GamepadButton
    {
    public:
      GamepadButton(bool pressed = false, bool touched = false, float value = 0.0f)
          : pressed_(pressed)
          , touched_(touched)
          , value_(value)
      {
      }

      bool pressed() const
      {
        return pressed_;
      }
      bool touched() const
      {
        return touched_;
      }
      float value() const
      {
        return value_;
      }

    private:
      bool pressed_;
      bool touched_;
      float value_;
    };

    /**
     * Represents a gamepad associated with an XRInputSource.
     * See: https://www.w3.org/TR/webxr-gamepads-module-1/
     */
    class Gamepad
    {
    public:
      explicit Gamepad(const xr::TrXRInputSource *inputSource);

      const std::string &id() const
      {
        return id_;
      }
      int64_t index() const
      {
        return index_;
      }
      bool connected() const
      {
        return connected_;
      }
      int64_t timestamp() const
      {
        return timestamp_;
      }
      const std::string &mapping() const
      {
        return mapping_;
      }
      const std::vector<float> &axes() const
      {
        return axes_;
      }
      const std::vector<GamepadButton> &buttons() const
      {
        return buttons_;
      }

      void update(const xr::TrXRInputSource *inputSource, int64_t timestamp);

    private:
      std::string id_;
      int64_t index_;
      bool connected_;
      int64_t timestamp_;
      std::string mapping_;
      std::vector<float> axes_;
      std::vector<GamepadButton> buttons_;
    };
  }
} // namespace endor
