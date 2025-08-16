#pragma once

#include <string>
#include <unordered_set>

namespace cdp_domains
{

  // Get the set of domains that should be forwarded to content processes
  inline std::unordered_set<std::string> getForwardedDomains()
  {
    return {
      "Runtime",
      "Log",
    };
  }

} // namespace cdp_domains