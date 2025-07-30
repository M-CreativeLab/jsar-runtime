#include "webnn_context.hpp"
#include "ggml_impl.hpp"
#include <stdexcept>
#include <memory>

namespace transmute {
namespace nn {

// Static factory method implementation
std::shared_ptr<WebNNContext> WebNNContext::Create(const WebNNContextOptions& options) {
  return std::make_shared<GgmlWebNNContext>(options);
}

} // namespace nn
} // namespace transmute