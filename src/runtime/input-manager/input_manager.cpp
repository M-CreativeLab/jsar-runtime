#include "./input_manager.hpp"
#include "../content.hpp"
#include "../content_manager.hpp"
#include "common/debug.hpp"
#include <chrono>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace input_manager;

TrInputManager::TrInputManager(std::shared_ptr<TrConstellation> constellation)
    : constellation_(constellation)
{
  TR_LOG_DEBUG("InputManager initialized");
}

bool TrInputManager::dispatchKeyboardEvent(const TrKeyboardEventData &eventData)
{
  if (!constellation_ || !constellation_->contentManager)
  {
    TR_LOG_ERROR("InputManager: No constellation or content manager available");
    return false;
  }

  return sendKeyboardEventToAllContents(eventData);
}

bool TrInputManager::sendKeyboardEventToAllContents(const TrKeyboardEventData &eventData)
{
  if (!constellation_ || !constellation_->contentManager)
  {
    TR_LOG_ERROR("InputManager: No constellation or content manager available");
    return false;
  }

  try
  {
    // Determine the input event type based on keyboard event type
    events_comm::TrInputEventType inputEventType;
    if (eventData.type == "keydown")
      inputEventType = events_comm::TrInputEventType::KeyboardDown;
    else if (eventData.type == "keyup")
      inputEventType = events_comm::TrInputEventType::KeyboardUp;
    else if (eventData.type == "keypress")
      inputEventType = events_comm::TrInputEventType::KeyboardPress;
    else
    {
      TR_LOG_WARNING("InputManager: Unknown keyboard event type: {}", eventData.type);
      return false;
    }

    // Create an input event with the serialized keyboard data
    events_comm::TrInputEvent inputEvent(
      0, // No specific content ID - this is a global event
      inputEventType,
      serializeKeyboardEvent(eventData));

    // Create a native event to send via IPC
    auto nativeEvent = std::make_shared<events_comm::TrNativeEvent>(events_comm::TrNativeEventType::InputEvent);
    nativeEvent->setDetail(inputEvent);

    // Dispatch to all active content runtimes via constellation
    // This will automatically handle broadcasting to all listening contents
    bool success = constellation_->dispatchNativeEvent(*nativeEvent, nullptr);

    if (success)
    {
      TR_LOG_DEBUG("InputManager: Keyboard event '{}' dispatched globally", eventData.type);
    }
    else
    {
      TR_LOG_WARNING("InputManager: Failed to dispatch keyboard event globally");
    }

    return success;
  }
  catch (const std::exception &e)
  {
    TR_LOG_ERROR("InputManager: Failed to send keyboard event: {}", e.what());
    return false;
  }
}

std::string TrInputManager::serializeKeyboardEvent(const TrKeyboardEventData &eventData)
{
  rapidjson::Document doc;
  doc.SetObject();
  auto &allocator = doc.GetAllocator();

  doc.AddMember("type", rapidjson::Value(eventData.type.c_str(), allocator), allocator);
  doc.AddMember("key", rapidjson::Value(eventData.key.c_str(), allocator), allocator);
  doc.AddMember("code", rapidjson::Value(eventData.code.c_str(), allocator), allocator);
  doc.AddMember("location", eventData.location, allocator);
  doc.AddMember("repeat", eventData.repeat, allocator);
  doc.AddMember("altKey", eventData.altKey, allocator);
  doc.AddMember("ctrlKey", eventData.ctrlKey, allocator);
  doc.AddMember("metaKey", eventData.metaKey, allocator);
  doc.AddMember("shiftKey", eventData.shiftKey, allocator);
  doc.AddMember("keyCode", eventData.keyCode, allocator);
  doc.AddMember("charCode", eventData.charCode, allocator);
  doc.AddMember("timestamp", eventData.timestamp, allocator);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);

  return buffer.GetString();
}

// Helper functions implementation
namespace input_manager::keyboard_events
{
  TrKeyboardEventData createKeyDownEvent(const std::string &key,
                                         const std::string &code,
                                         bool altKey,
                                         bool ctrlKey,
                                         bool metaKey,
                                         bool shiftKey)
  {
    TrKeyboardEventData eventData;
    eventData.type = "keydown";
    eventData.key = key;
    eventData.code = code;
    eventData.location = 0; // Standard location by default
    eventData.repeat = false;
    eventData.altKey = altKey;
    eventData.ctrlKey = ctrlKey;
    eventData.metaKey = metaKey;
    eventData.shiftKey = shiftKey;
    eventData.keyCode = 0; // Will be set based on key
    eventData.charCode = 0;

    auto now = std::chrono::system_clock::now();
    eventData.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now.time_since_epoch())
                            .count();

    return eventData;
  }

  TrKeyboardEventData createKeyUpEvent(const std::string &key,
                                       const std::string &code,
                                       bool altKey,
                                       bool ctrlKey,
                                       bool metaKey,
                                       bool shiftKey)
  {
    TrKeyboardEventData eventData;
    eventData.type = "keyup";
    eventData.key = key;
    eventData.code = code;
    eventData.location = 0;
    eventData.repeat = false;
    eventData.altKey = altKey;
    eventData.ctrlKey = ctrlKey;
    eventData.metaKey = metaKey;
    eventData.shiftKey = shiftKey;
    eventData.keyCode = 0;
    eventData.charCode = 0;

    auto now = std::chrono::system_clock::now();
    eventData.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now.time_since_epoch())
                            .count();

    return eventData;
  }

  TrKeyboardEventData createKeyPressEvent(const std::string &key,
                                          const std::string &code,
                                          bool altKey,
                                          bool ctrlKey,
                                          bool metaKey,
                                          bool shiftKey)
  {
    TrKeyboardEventData eventData;
    eventData.type = "keypress";
    eventData.key = key;
    eventData.code = code;
    eventData.location = 0;
    eventData.repeat = false;
    eventData.altKey = altKey;
    eventData.ctrlKey = ctrlKey;
    eventData.metaKey = metaKey;
    eventData.shiftKey = shiftKey;
    eventData.keyCode = 0;
    eventData.charCode = 0;

    auto now = std::chrono::system_clock::now();
    eventData.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now.time_since_epoch())
                            .count();

    return eventData;
  }
}