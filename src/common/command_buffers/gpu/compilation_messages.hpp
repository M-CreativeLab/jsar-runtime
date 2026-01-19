#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <common/command_buffers/gpu/gpu_base.hpp>

namespace commandbuffers::gpu
{
  struct CompilationMessageContent
  {
    std::string message;
    GPUCompilationMessageType type;
    uint64_t line;
    uint64_t column;
  };

  struct ParsedCompilationMessages
  {
    std::vector<CompilationMessageContent> messages;
    std::vector<std::string> formattedTintMessages;
  };
}
