#pragma once

#include <string>
#include <rapidjson/document.h>

#include "./event_type.hpp"
#include "./classes.hpp"

using namespace std;

namespace events
{
  class TrXSMLRequestInit
  {
  public:
    TrXSMLRequestInit(string url, uint32_t id) : url(url), id(id)
    {
    }
    TrXSMLRequestInit(const TrXSMLRequestInit &that)
        : url(that.url),
          id(that.id),
          disableCache(that.disableCache),
          isPreview(that.isPreview),
          runScripts(that.runScripts)
    {
    }

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
    TrEventDetail(string jsonSource) : jsonSource(jsonSource)
    {
    }
    ~TrEventDetail()
    {
    }

  public:
    TrXSMLRequestInit get()
    {
      rapidjson::Document jsonDoc;
      jsonDoc.Parse(jsonSource.c_str());
      auto url = jsonDoc["url"].GetString();
      auto id = jsonDoc["sessionId"].GetUint();
      return TrXSMLRequestInit(url, id);
    }

  private:
    string jsonSource;
  };

  class TrEvent
  {
  public:
    TrEvent(int id, TrEventType type, string detailData) : id(id), type(type), detail(TrEventDetail(detailData))
    {
    }

  public:
    int id;
    TrEventType type;
    TrEventDetail detail;
  };
}
