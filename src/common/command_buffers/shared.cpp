#include "./shared.hpp"
#include "./macros.hpp"

namespace commandbuffers
{
  std::string commandTypeToStr(CommandBufferType type)
  {
    switch (type)
    {
#define XX(commandType, requestType)       \
  case COMMAND_BUFFER_##commandType##_REQ: \
  {                                        \
    return #requestType;                   \
  }
      TR_COMMAND_BUFFER_REQUESTS_MAP(XX)
#undef XX
    case COMMAND_BUFFER_UNKNOWN:
    default:
      return "Unknown";
    }
  }
}
