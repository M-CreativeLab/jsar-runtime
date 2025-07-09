#include "./shared.hpp"
#include "./macros.hpp"

namespace commandbuffers
{
  std::string commandTypeToStr(CommandBufferType type)
  {
    switch (type)
    {

// Request
#define XX(COMMAND_TYPE, STR)               \
  case COMMAND_BUFFER_##COMMAND_TYPE##_REQ: \
  {                                         \
    return #STR;                            \
  }
      TR_COMMAND_BUFFER_REQUESTS_MAP(XX)
#undef XX

// Response
#define XX(COMMAND_TYPE, STR)               \
  case COMMAND_BUFFER_##COMMAND_TYPE##_RES: \
  {                                         \
    return #STR;                            \
  }
      TR_COMMAND_BUFFER_RESPONSES_MAP(XX)
#undef XX

    case COMMAND_BUFFER_UNKNOWN:
    default:
      return "Unknown";
    }
  }
}
