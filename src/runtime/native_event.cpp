#include "native_event.hpp"

namespace native_event
{
  TrXSMLRequestInit::TrXSMLRequestInit(string url, uint32_t id)
      : url(url), id(id)
  {
  }

  TrXSMLRequestInit::TrXSMLRequestInit(const TrXSMLRequestInit &that)
      : url(that.url),
        id(that.id),
        disableCache(that.disableCache),
        isPreview(that.isPreview),
        runScripts(that.runScripts)
  {
  }

  TrEventDetail::TrEventDetail(const char *data)
  {
    jsonSource = string(data);
  }

  TrEventDetail::~TrEventDetail()
  {
    jsonSource.clear();
  }

  template <>
  TrXSMLRequestInit TrEventDetail::get()
  {
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(jsonSource.c_str());

    auto url = jsonDoc["url"].GetString();
    auto id = jsonDoc["sessionId"].GetUint();
    return TrXSMLRequestInit(url, id);
  }

  TrNativeEvent::TrNativeEvent(int id, TrEventType type, const char *data)
      : id(id), type(type), detail(TrEventDetail(data))
  {
  }

  TrIdGenerator *TrEventListener::s_IdGenerator = new TrIdGenerator(1);
  TrEventListener::TrEventListener(TrEventCallback onEvent)
      : onEvent(onEvent), id(s_IdGenerator->get())
  {
  }

  void TrEventListener::operator()(TrEventType type, TrNativeEvent &event)
  {
    onEvent(type, event);
  }


  TrNativeEventTarget::TrNativeEventTarget()
  {
  }

  TrNativeEventTarget::~TrNativeEventTarget()
  {
  }

  bool TrNativeEventTarget::dispatchEvent(int id, TrEventType type, const char *data)
  {
    TrNativeEvent event(id, type, data);
    auto it = listeners.find(type);
    if (it != listeners.end())
    {
      auto listener = *it->second;
      listener(type, event);
    }
    return true;
  }

  TrEventListener *TrNativeEventTarget::addEventListener(TrEventType type, TrEventCallback listenerCallback)
  {
    auto listener = new TrEventListener(listenerCallback);
    listeners.insert({type, listener});
    return listener;
  }

  void TrNativeEventTarget::removeEventListener(TrEventListener *listener)
  {
    for (auto it = listeners.begin(); it != listeners.end();)
    {
      if (listener == it->second)
      {
        delete listener;
        it = listeners.erase(it);
        break;
      }
      else
      {
        ++it;
      }
    }
  }

  void TrNativeEventTarget::removeEventListener(TrEventType type)
  {
    for (auto it = listeners.begin(); it != listeners.end();)
    {
      if (it->first == type)
      {
        delete it->second;
        it = listeners.erase(it);
      }
      else
      {
        ++it;
      }
    }
  }
}
