#include "ggml_impl.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace transmute {
namespace nn {

// GgmlWebNNContext implementation
GgmlWebNNContext::GgmlWebNNContext(const WebNNContextOptions& options) 
  : options_(options), ctx_(nullptr), backend_(nullptr) {
  
  // Initialize ggml context with a reasonable memory budget
  struct ggml_init_params params = {
    /*.mem_size   =*/ 128 * 1024 * 1024, // 128 MB
    /*.mem_buffer =*/ nullptr,
    /*.no_alloc   =*/ false,
  };
  
  ctx_ = ggml_init(params);
  if (!ctx_) {
    throw std::runtime_error("Failed to initialize GGML context");
  }
  
  // Initialize backend based on device type
  switch (options_.deviceType) {
    case WebNNDeviceType::CPU:
      backend_ = ggml_backend_init_by_type(GGML_BACKEND_DEVICE_TYPE_CPU, nullptr);
      break;
    case WebNNDeviceType::GPU:
      // Try to initialize GPU backend, fallback to CPU if not available
      backend_ = ggml_backend_init_by_type(GGML_BACKEND_DEVICE_TYPE_GPU, nullptr);
      if (!backend_) {
        backend_ = ggml_backend_init_by_type(GGML_BACKEND_DEVICE_TYPE_CPU, nullptr);
      }
      break;
    case WebNNDeviceType::NPU:
      // NPU not supported yet, fallback to CPU
      backend_ = ggml_backend_init_by_type(GGML_BACKEND_DEVICE_TYPE_CPU, nullptr);
      break;
    default:
      backend_ = ggml_backend_init_by_type(GGML_BACKEND_DEVICE_TYPE_CPU, nullptr);
      break;
  }
  
  if (!backend_) {
    ggml_free(ctx_);
    throw std::runtime_error("Failed to initialize GGML backend");
  }
}

GgmlWebNNContext::~GgmlWebNNContext() {
  if (backend_) {
    ggml_backend_free(backend_);
  }
  if (ctx_) {
    ggml_free(ctx_);
  }
}

ggml_type GgmlWebNNContext::getGgmlType(const std::string& dataType) const {
  if (dataType == "float32") return GGML_TYPE_F32;
  if (dataType == "float16") return GGML_TYPE_F16;
  if (dataType == "int32") return GGML_TYPE_I32;
  if (dataType == "int16") return GGML_TYPE_I16;
  if (dataType == "int8") return GGML_TYPE_I8;
  if (dataType == "uint8") return GGML_TYPE_I8; // Note: GGML doesn't have uint8, using int8
  
  throw std::invalid_argument("Unsupported data type: " + dataType);
}

size_t GgmlWebNNContext::getTypeSize(const std::string& dataType) const {
  if (dataType == "float32") return 4;
  if (dataType == "float16") return 2;
  if (dataType == "int32") return 4;
  if (dataType == "int16") return 2;
  if (dataType == "int8" || dataType == "uint8") return 1;
  
  throw std::invalid_argument("Unsupported data type: " + dataType);
}

std::shared_ptr<WebNNOperand> GgmlWebNNContext::input(const std::string& name, const WebNNOperandDescriptor& descriptor) {
  ggml_type type = getGgmlType(descriptor.dataType);
  
  // Convert dimensions to int64_t array for ggml
  std::vector<int64_t> dims(descriptor.dimensions.begin(), descriptor.dimensions.end());
  
  ggml_tensor* tensor = nullptr;
  switch (dims.size()) {
    case 1:
      tensor = ggml_new_tensor_1d(ctx_, type, dims[0]);
      break;
    case 2:
      tensor = ggml_new_tensor_2d(ctx_, type, dims[0], dims[1]);
      break;
    case 3:
      tensor = ggml_new_tensor_3d(ctx_, type, dims[0], dims[1], dims[2]);
      break;
    case 4:
      tensor = ggml_new_tensor_4d(ctx_, type, dims[0], dims[1], dims[2], dims[3]);
      break;
    default:
      throw std::invalid_argument("Unsupported number of dimensions: " + std::to_string(dims.size()));
  }
  
  if (!tensor) {
    throw std::runtime_error("Failed to create input tensor");
  }
  
  auto operand = std::make_shared<GgmlWebNNOperand>(descriptor, tensor);
  inputs_[name] = operand;
  return operand;
}

std::shared_ptr<WebNNOperand> GgmlWebNNContext::constant(const WebNNOperandDescriptor& descriptor, const void* data) {
  ggml_type type = getGgmlType(descriptor.dataType);
  
  // Convert dimensions to int64_t array for ggml
  std::vector<int64_t> dims(descriptor.dimensions.begin(), descriptor.dimensions.end());
  
  ggml_tensor* tensor = nullptr;
  switch (dims.size()) {
    case 1:
      tensor = ggml_new_tensor_1d(ctx_, type, dims[0]);
      break;
    case 2:
      tensor = ggml_new_tensor_2d(ctx_, type, dims[0], dims[1]);
      break;
    case 3:
      tensor = ggml_new_tensor_3d(ctx_, type, dims[0], dims[1], dims[2]);
      break;
    case 4:
      tensor = ggml_new_tensor_4d(ctx_, type, dims[0], dims[1], dims[2], dims[3]);
      break;
    default:
      throw std::invalid_argument("Unsupported number of dimensions: " + std::to_string(dims.size()));
  }
  
  if (!tensor) {
    throw std::runtime_error("Failed to create constant tensor");
  }
  
  // Copy data into tensor
  size_t data_size = ggml_nbytes(tensor);
  std::memcpy(tensor->data, data, data_size);
  
  return std::make_shared<GgmlWebNNOperand>(descriptor, tensor);
}

std::shared_ptr<WebNNOperand> GgmlWebNNContext::add(std::shared_ptr<WebNNOperand> a, std::shared_ptr<WebNNOperand> b) {
  auto ggml_a = std::static_pointer_cast<GgmlWebNNOperand>(a);
  auto ggml_b = std::static_pointer_cast<GgmlWebNNOperand>(b);
  
  ggml_tensor* result = ggml_add(ctx_, ggml_a->getTensor(), ggml_b->getTensor());
  if (!result) {
    throw std::runtime_error("Failed to create add operation");
  }
  
  // Create descriptor for result (same as input a for element-wise operations)
  WebNNOperandDescriptor result_desc = a->descriptor();
  return std::make_shared<GgmlWebNNOperand>(result_desc, result);
}

std::shared_ptr<WebNNOperand> GgmlWebNNContext::mul(std::shared_ptr<WebNNOperand> a, std::shared_ptr<WebNNOperand> b) {
  auto ggml_a = std::static_pointer_cast<GgmlWebNNOperand>(a);
  auto ggml_b = std::static_pointer_cast<GgmlWebNNOperand>(b);
  
  ggml_tensor* result = ggml_mul(ctx_, ggml_a->getTensor(), ggml_b->getTensor());
  if (!result) {
    throw std::runtime_error("Failed to create mul operation");
  }
  
  WebNNOperandDescriptor result_desc = a->descriptor();
  return std::make_shared<GgmlWebNNOperand>(result_desc, result);
}

std::shared_ptr<WebNNOperand> GgmlWebNNContext::matmul(std::shared_ptr<WebNNOperand> a, std::shared_ptr<WebNNOperand> b) {
  auto ggml_a = std::static_pointer_cast<GgmlWebNNOperand>(a);
  auto ggml_b = std::static_pointer_cast<GgmlWebNNOperand>(b);
  
  ggml_tensor* result = ggml_mul_mat(ctx_, ggml_a->getTensor(), ggml_b->getTensor());
  if (!result) {
    throw std::runtime_error("Failed to create matmul operation");
  }
  
  // Calculate result dimensions for matrix multiplication
  const auto& a_desc = a->descriptor();
  const auto& b_desc = b->descriptor();
  
  WebNNOperandDescriptor result_desc;
  result_desc.dataType = a_desc.dataType;
  
  // For 2D matrix multiplication: [M, K] x [K, N] = [M, N]
  if (a_desc.dimensions.size() == 2 && b_desc.dimensions.size() == 2) {
    result_desc.dimensions = {a_desc.dimensions[0], b_desc.dimensions[1]};
  } else {
    // For simplicity, just copy the dimensions from a for now
    result_desc.dimensions = a_desc.dimensions;
  }
  
  return std::make_shared<GgmlWebNNOperand>(result_desc, result);
}

std::shared_ptr<WebNNOperand> GgmlWebNNContext::relu(std::shared_ptr<WebNNOperand> input) {
  auto ggml_input = std::static_pointer_cast<GgmlWebNNOperand>(input);
  
  ggml_tensor* result = ggml_relu(ctx_, ggml_input->getTensor());
  if (!result) {
    throw std::runtime_error("Failed to create relu operation");
  }
  
  WebNNOperandDescriptor result_desc = input->descriptor();
  return std::make_shared<GgmlWebNNOperand>(result_desc, result);
}

std::shared_ptr<WebNNOperand> GgmlWebNNContext::softmax(std::shared_ptr<WebNNOperand> input) {
  auto ggml_input = std::static_pointer_cast<GgmlWebNNOperand>(input);
  
  ggml_tensor* result = ggml_soft_max(ctx_, ggml_input->getTensor());
  if (!result) {
    throw std::runtime_error("Failed to create softmax operation");
  }
  
  WebNNOperandDescriptor result_desc = input->descriptor();
  return std::make_shared<GgmlWebNNOperand>(result_desc, result);
}

std::shared_ptr<WebNNGraph> GgmlWebNNContext::build(const std::map<std::string, std::shared_ptr<WebNNOperand>>& outputs) {
  return std::make_shared<GgmlWebNNGraph>(ctx_, backend_, outputs, inputs_);
}

// GgmlWebNNOperand implementation
GgmlWebNNOperand::GgmlWebNNOperand(const WebNNOperandDescriptor& descriptor, ggml_tensor* tensor)
  : descriptor_(descriptor), tensor_(tensor) {
}

// GgmlWebNNGraph implementation
GgmlWebNNGraph::GgmlWebNNGraph(
  ggml_context* ctx,
  ggml_backend_t backend,
  const std::map<std::string, std::shared_ptr<WebNNOperand>>& outputs,
  const std::unordered_map<std::string, std::shared_ptr<GgmlWebNNOperand>>& inputs
) : ctx_(ctx), backend_(backend), outputs_(outputs), inputs_(inputs) {
  
  // Create computation graph
  cgraph_ = ggml_new_graph(ctx_);
  if (!cgraph_) {
    throw std::runtime_error("Failed to create computation graph");
  }
  
  // Add output tensors to the graph
  for (const auto& [name, operand] : outputs_) {
    auto ggml_operand = std::static_pointer_cast<GgmlWebNNOperand>(operand);
    ggml_build_forward_expand(cgraph_, ggml_operand->getTensor());
  }
}

GgmlWebNNGraph::~GgmlWebNNGraph() {
  // ggml_context cleanup will handle the graph
}

std::shared_ptr<WebNNComputeResult> GgmlWebNNGraph::compute(const std::map<std::string, const void*>& inputs) {
  // Set input data
  for (const auto& [name, data] : inputs) {
    auto input_it = inputs_.find(name);
    if (input_it != inputs_.end()) {
      ggml_tensor* tensor = input_it->second->getTensor();
      size_t data_size = ggml_nbytes(tensor);
      std::memcpy(tensor->data, data, data_size);
    }
  }
  
  // Execute computation using backend
  ggml_status status = ggml_backend_graph_compute(backend_, cgraph_);
  if (status != GGML_STATUS_SUCCESS) {
    throw std::runtime_error("Graph computation failed");
  }
  
  return std::make_shared<GgmlWebNNComputeResult>(outputs_);
}

// GgmlWebNNComputeResult implementation
GgmlWebNNComputeResult::GgmlWebNNComputeResult(const std::map<std::string, std::shared_ptr<WebNNOperand>>& outputs)
  : outputs_(outputs) {
}

const void* GgmlWebNNComputeResult::getOutput(const std::string& name) const {
  auto it = outputs_.find(name);
  if (it != outputs_.end()) {
    auto ggml_operand = std::static_pointer_cast<GgmlWebNNOperand>(it->second);
    return ggml_operand->getTensor()->data;
  }
  return nullptr;
}

std::vector<std::string> GgmlWebNNComputeResult::getOutputNames() const {
  std::vector<std::string> names;
  for (const auto& [name, operand] : outputs_) {
    names.push_back(name);
  }
  return names;
}

} // namespace nn
} // namespace transmute