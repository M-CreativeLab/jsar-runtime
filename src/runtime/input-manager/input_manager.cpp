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

bool TrInputManager::injectKeyboardEvent(uint32_t contentId, const TrKeyboardEventData &eventData)
{
  if (!constellation_ || !constellation_->contentManager)
  {
    TR_LOG_ERROR("InputManager: No constellation or content manager available");
    return false;
  }

  auto content = constellation_->contentManager->getContent(contentId);
  if (!content)
  {
    TR_LOG_WARNING("InputManager: Content runtime with ID {} not found", contentId);
    return false;
  }

  return sendKeyboardEventToContent(content, eventData);
}

int TrInputManager::broadcastKeyboardEvent(const TrKeyboardEventData &eventData)
{
  if (!constellation_ || !constellation_->contentManager)
  {
    TR_LOG_ERROR("InputManager: No constellation or content manager available");
    return 0;
  }

  // Since we don't have a direct getActiveContents method,
  // we'll need to iterate through available contents
  // For now, implement with the available API
  TR_LOG_DEBUG("InputManager: Broadcasting keyboard event (method needs implementation)");
  return 0; // TODO: Implement when we have access to all contents
}

std::vector<uint32_t> TrInputManager::getActiveContentIds() const
{
  std::vector<uint32_t> contentIds;

  if (!constellation_ || !constellation_->contentManager)
  {
    return contentIds;
  }

  // TODO: Need to implement with available content manager API
  return contentIds;
}

bool TrInputManager::isContentActive(uint32_t contentId) const
{
  if (!constellation_ || !constellation_->contentManager)
  {
    return false;
  }

  auto content = constellation_->contentManager->getContent(contentId);
  return content != nullptr; // Basic check if content exists
}

bool TrInputManager::sendKeyboardEventToContent(std::shared_ptr<TrContentRuntime> content,
                                                const TrKeyboardEventData &eventData)
{
  if (!content)
  {
    TR_LOG_WARNING("InputManager: Content runtime is null");
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
      content->getId(),
      inputEventType,
      serializeKeyboardEvent(eventData));

    // Create a native event to send via IPC
    auto nativeEvent = std::make_shared<events_comm::TrNativeEvent>(events_comm::TrNativeEventType::InputEvent);
    nativeEvent->setDetail(inputEvent);

    // Send the event to the content runtime
    bool success = content->dispatchEvent(nativeEvent);

    if (success)
    {
      TR_LOG_DEBUG("InputManager: Keyboard event '{}' sent to content {}",
                   eventData.type,
                   content->getId());
    }
    else
    {
      TR_LOG_WARNING("InputManager: Failed to dispatch keyboard event to content {}",
                     content->getId());
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