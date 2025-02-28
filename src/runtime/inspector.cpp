#include <memory>
#include <rapidjson/document.h>

#include "./inspector.hpp"
#include "./constellation.hpp"
#include "./content_manager.hpp"

using namespace std;

void TrInspector::initialize()
{
  server_ = make_unique<TrInspectorServer>(shared_from_this());
}

void TrInspector::tick()
{
  server_->tryAccept();
  server_->tick();
}

void TrInspector::onRequest(TrInspectorClient &requestClient)
{
  if (requestClient.url() == "/contents")
  {
    rapidjson::Document json;
    json.SetArray();
    auto &allocator = json.GetAllocator();
    {
      auto contentMgr = constellation->contentManager;
      for (auto content : contentMgr->contents)
      {
        rapidjson::Value contentJson;
        contentJson.SetObject();
        contentJson.AddMember("id", content->id, allocator);
        contentJson.AddMember("pid", content->pid.load(), allocator);
        contentJson.AddMember("used", content->used.load(), allocator);
        {
          rapidjson::Value requestInitJson;
          requestInitJson.SetObject();

          auto &requestInit = content->requestInit;
          requestInitJson.AddMember("url",
                                    rapidjson::Value().SetString(requestInit.url.c_str(), allocator), allocator);
          requestInitJson.AddMember("disableCache", requestInit.disableCache, allocator);
          contentJson.AddMember("requestInit", requestInitJson, allocator);
        }
        json.PushBack(contentJson, allocator);
      }
    }
    requestClient.respond(200, json);
  }
  else
  {
    requestClient.respond(404, "Not Found");
  }
}
