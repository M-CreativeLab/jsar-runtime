#pragma once

#include <map>
#include <string>
#include <functional>
#include <rapidjson/document.h>
#include "idgen.hpp"

using namespace std;

namespace native_event
{
  enum TrEventType
  {
    TrRpcRequest = 0x100,
    TrRpcResponse = 0x101,
    TrMessage = 0x200,
    TrError = 0x210,
    TrClose = 0x220,
    TrXSMLRequest = 0x300,
    TrXSMLEvent = 0x301,
  };

  class TrNativeEvent;
  class TrNativeEventTarget;
  class TrEventListener;

  class TrXSMLRequestInit
  {
  public:
    TrXSMLRequestInit(string url, uint32_t id);
    TrXSMLRequestInit(const TrXSMLRequestInit &that);

  public:
    string url;
    uint32_t id;
    bool disableCache;
    bool isPreview;
    string runScripts;
  };

  class TrEventDetail
  {
  public:
    TrEventDetail(const char *data);
    ~TrEventDetail();

  public:
    template <typename T>
    T get();

  private:
    string jsonSource;
  };

  class TrNativeEvent
  {
  public:
    TrNativeEvent(int id, TrEventType type, const char *data);

  public:
    int id;
    TrEventType type;
    TrEventDetail detail;
  };

  using TrEventCallback = function<void(TrEventType type, TrNativeEvent &event)>;
  class TrEventListener
  {
  public:
    TrEventListener(TrEventCallback onEvent);
    void operator()(TrEventType type, TrNativeEvent &event);

  private:
    uint32_t id;
    TrEventCallback onEvent;

  private:
    static TrIdGenerator *s_IdGenerator;
  };

  class TrNativeEventTarget
  {
  private:
    static TrNativeEventTarget *s_Instance;

  public:
    static TrNativeEventTarget *GetOrCreateInstance();

  private:
    TrNativeEventTarget();
    ~TrNativeEventTarget();

  public:
    bool dispatchEvent(int id, TrEventType type, const char *data);
    TrEventListener *addEventListener(TrEventType type, TrEventCallback listenerCallback);
    void removeEventListener(TrEventListener *listener);
    void removeEventListener(TrEventType type);

  protected:
    map<TrEventType, TrEventListener *> listeners;
  };
}
