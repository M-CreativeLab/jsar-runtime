#include <rapidjson/document.h>
#include "./runtime_domain.hpp"

namespace client_inspector::domains
{
  using namespace std;

  CdpRuntimeDomain::CdpRuntimeDomain()
  {
  }

  string CdpRuntimeDomain::handleMethod(const string &method, const CdpMessage &message)
  {
    if (method == "enable")
    {
      return enable(message);
    }
    else if (method == "disable")
    {
      return disable(message);
    }
    else if (method == "getVersion")
    {
      return getVersion(message);
    }

    return CdpResponse::error(message.id, -32601, "Runtime." + method + " is not supported in content process");
  }

  string CdpRuntimeDomain::enable(const CdpMessage &message)
  {
    enabled_ = true;

    rapidjson::Document result;
    result.SetObject();
    return CdpResponse::success(message.id, result);
  }

  string CdpRuntimeDomain::disable(const CdpMessage &message)
  {
    enabled_ = false;

    rapidjson::Document result;
    result.SetObject();
    return CdpResponse::success(message.id, result);
  }

  string CdpRuntimeDomain::getVersion(const CdpMessage &message)
  {
    rapidjson::Document result;
    result.SetObject();
    auto &allocator = result.GetAllocator();

    result.AddMember("product", rapidjson::Value("JSAR-Content", allocator), allocator);
    result.AddMember("revision", rapidjson::Value("0.9.0", allocator), allocator);
    result.AddMember("userAgent", rapidjson::Value("JSAR-Content/0.9.0", allocator), allocator);
    result.AddMember("jsVersion", rapidjson::Value("ES2021", allocator), allocator);
    result.AddMember("processType", rapidjson::Value("content", allocator), allocator);
    return CdpResponse::success(message.id, result);
  }
}
