#include <array>
#include <string>
#include "./mouse_event.hpp"

namespace dom::events
{
  bool MouseEvent::getModifierState(const string &key) const
  {
    static const array<string, 4> modifier_keys = {"Alt", "Control", "Meta", "Shift"};
    for (size_t i = 0; i < modifier_keys.size(); ++i)
    {
      if (key == modifier_keys[i])
        return (i == 0 && alt_key_) ||
               (i == 1 && ctrl_key_) ||
               (i == 2 && meta_key_) ||
               (i == 3 && shift_key_);
    }
    return false;
  }
}
