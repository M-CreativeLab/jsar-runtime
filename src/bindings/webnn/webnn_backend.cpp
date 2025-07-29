/**
 * WebNN Backend Implementation for JSAR
 */

#include "webnn_backend.hpp"
#include <stdexcept>

namespace jsar {
namespace webnn {

std::shared_ptr<MLContext> createMLContext(MLDeviceType deviceType) {
    auto context = std::make_shared<MLContext>(deviceType);
    
    // TODO: Initialize ggml context here
    // if (deviceType == MLDeviceType::CPU) {
    //     // Initialize ggml CPU context
    // } else if (deviceType == MLDeviceType::GPU) {
    //     // Initialize ggml GPU context if available
    // }
    
    return context;
}

std::shared_ptr<MLGraphBuilder> createMLGraphBuilder(std::shared_ptr<MLContext> context) {
    if (!context) {
        throw std::invalid_argument("MLContext cannot be null");
    }
    return std::make_shared<MLGraphBuilder>(context);
}

} // namespace webnn
} // namespace jsar