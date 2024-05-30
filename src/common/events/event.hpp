#pragma once

#include <string>
#include <rapidjson/document.h>

#include "idgen.hpp"
#include "./classes.hpp"
#include "./event_type.hpp"

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
    string &getString()
    {
      return jsonSource;
    }
    TrXSMLRequestInit asXSMLRequestInit()
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

  static TrIdGenerator eventIdGenerator(1);
  class TrEvent
  {
  public:
    TrEvent(TrEventType type, string detailData)
        : id(eventIdGenerator.get()), type(type), detail(TrEventDetail(detailData))
    {
    }
    TrEvent(int id, TrEventType type, string detailData)
        : id(id), type(type), detail(TrEventDetail(detailData))
    {
    }

  public:
    int id;
    TrEventType type;
    TrEventDetail detail;
  };
}
