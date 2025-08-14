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
   * to the appropriate JSAR content instances via IPC.
   * 
   * This class provides an abstraction layer between platform-specific input
   * capture (Unity, Unreal, native, etc.) and JSAR Runtime's event system.
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
     * Inject a keyboard event from the host application.
     * This method converts the host keyboard event into JSAR's internal format
     * and forwards it to the appropriate content runtime.
     * 
     * @param contentId The content runtime ID to send the event to
     * @param eventData The keyboard event data
     * @return true if the event was successfully injected, false otherwise
     */
    bool injectKeyboardEvent(uint32_t contentId, const TrKeyboardEventData &eventData);

    /**
     * Inject a keyboard event to all active content runtimes.
     * 
     * @param eventData The keyboard event data
     * @return number of content runtimes that received the event
     */
    int broadcastKeyboardEvent(const TrKeyboardEventData &eventData);

    /**
     * Get the list of active content runtime IDs that can receive input events.
     * 
     * @return vector of content runtime IDs
     */
    std::vector<uint32_t> getActiveContentIds() const;

    /**
     * Check if a specific content runtime is active and can receive events.
     * 
     * @param contentId The content runtime ID to check
     * @return true if the content runtime is active, false otherwise
     */
    bool isContentActive(uint32_t contentId) const;

  private:
    /**
     * Send keyboard event to a specific content runtime via IPC.
     * 
     * @param content The content runtime to send to
     * @param eventData The keyboard event data
     * @return true if successful, false otherwise
     */
    bool sendKeyboardEventToContent(std::shared_ptr<TrContentRuntime> content,
                                    const TrKeyboardEventData &eventData);

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