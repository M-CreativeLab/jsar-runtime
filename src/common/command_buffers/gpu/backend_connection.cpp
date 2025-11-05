#include <common/command_buffers/gpu/backend_connection.hpp>

namespace commandbuffers::gpu
{
  BackendConnection::BackendConnection(GPUInstance *instance, GPUBackendType type)
      : instance_(instance)
      , type_(type)
  {
  }

  GPUBackendType BackendConnection::type() const
  {
    return type_;
  }

  GPUInstance *BackendConnection::getInstance() const
  {
    return instance_;
  }
}
