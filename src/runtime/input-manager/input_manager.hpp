#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/events_v2/native_event.hpp"
#include "common/events_v2/event_target.hpp"
#include "../constellation.hpp"

// Forward declarations
class TrContentRuntime;

namespace input_manager
{
  /**
   * Keyboard event data structure for IPC communication
   */
  struct TrKeyboardEventData
  {
    std::string type;   // "keydown", "keyup", "keypress"
    std::string key;    // Key value (e.g., "a", "Enter", "Escape")
    std::string code;   // Physical key code (e.g., "KeyA", "Enter", "Escape")
    int location;       // Key location (standard, left, right, numpad)
    bool repeat;        // Whether key is being held down
    bool altKey;        // Alt modifier state
    bool ctrlKey;       // Ctrl modifier state
    bool metaKey;       // Meta modifier state
    bool shiftKey;      // Shift modifier state
    int keyCode;        // Legacy keyCode value
    int charCode;       // Legacy charCode value
    uint32_t timestamp; // Event timestamp
  };

  /**
   * InputManager handles input events from host applications and forwards them
   * to all active JSAR content instances via IPC.
   * 
   * This class provides an abstraction layer between platform-specific input
   * capture (Unity, Unreal, native, etc.) and JSAR Runtime's event system.
   * Input events are globally dispatched to all listening content runtimes.
   */
  class TrInputManager
  {
  public:
    /**
     * Construct a new Input Manager instance.
     * 
     * @param constellation The constellation instance for IPC communication
     */
    explicit TrInputManager(std::shared_ptr<TrConstellation> constellation);
    virtual ~TrInputManager() = default;

  public:
    /**
     * Dispatch a keyboard event from the host application to all active content runtimes.
     * This method converts the host keyboard event into JSAR's internal format
     * and forwards it to all listening content instances.
     * 
     * @param eventData The keyboard event data
     * @return true if the event was successfully dispatched, false otherwise
     */
    bool dispatchKeyboardEvent(const TrKeyboardEventData &eventData);

  private:
    /**
     * Send keyboard event to all active content runtimes via IPC.
     * 
     * @param eventData The keyboard event data
     * @return true if successful, false otherwise
     */
    bool sendKeyboardEventToAllContents(const TrKeyboardEventData &eventData);

    /**
     * Convert keyboard event data to JSON for IPC transmission.
     * 
     * @param eventData The keyboard event data
     * @return JSON string representation
     */
    std::string serializeKeyboardEvent(const TrKeyboardEventData &eventData);

  private:
    std::shared_ptr<TrConstellation> constellation_;
  };

  /**
   * Helper functions for keyboard event creation
   */
  namespace keyboard_events
  {
    /**
     * Create a keyboard event data structure for a key down event.
     * 
     * @param key The key value
     * @param code The key code
     * @param modifiers Modifier key states (alt, ctrl, meta, shift)
     * @return TrKeyboardEventData structure
     */
    TrKeyboardEventData createKeyDownEvent(const std::string &key,
                                           const std::string &code,
                                           bool altKey = false,
                                           bool ctrlKey = false,
                                           bool metaKey = false,
                                           bool shiftKey = false);

    /**
     * Create a keyboard event data structure for a key up event.
     * 
     * @param key The key value
     * @param code The key code
     * @param modifiers Modifier key states (alt, ctrl, meta, shift)
     * @return TrKeyboardEventData structure
     */
    TrKeyboardEventData createKeyUpEvent(const std::string &key,
                                         const std::string &code,
                                         bool altKey = false,
                                         bool ctrlKey = false,
                                         bool metaKey = false,
                                         bool shiftKey = false);

    /**
     * Create a keyboard event data structure for a key press event.
     * 
     * @param key The key value
     * @param code The key code
     * @param modifiers Modifier key states (alt, ctrl, meta, shift)
     * @return TrKeyboardEventData structure
     */
    TrKeyboardEventData createKeyPressEvent(const std::string &key,
                                            const std::string &code,
                                            bool altKey = false,
                                            bool ctrlKey = false,
                                            bool metaKey = false,
                                            bool shiftKey = false);
  }
}