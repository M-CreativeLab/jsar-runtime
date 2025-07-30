#pragma once

#include "webnn_context.hpp"
#include <ggml.h>
#include <ggml-backend.h>
#include <unordered_map>

namespace transmute {
namespace nn {

// Forward declarations
class GgmlWebNNOperand;
class GgmlWebNNGraph;
class GgmlWebNNComputeResult;

// GGML implementation of WebNN Context
class GgmlWebNNContext : public WebNNContext {
public:
  explicit GgmlWebNNContext(const WebNNContextOptions& options);
  ~GgmlWebNNContext() override;
  
  // WebNNContext implementation
  std::shared_ptr<WebNNOperand> input(const std::string& name, const WebNNOperandDescriptor& descriptor) override;
  std::shared_ptr<WebNNOperand> constant(const WebNNOperandDescriptor& descriptor, const void* data) override;
  
  std::shared_ptr<WebNNOperand> add(std::shared_ptr<WebNNOperand> a, std::shared_ptr<WebNNOperand> b) override;
  std::shared_ptr<WebNNOperand> mul(std::shared_ptr<WebNNOperand> a, std::shared_ptr<WebNNOperand> b) override;
  std::shared_ptr<WebNNOperand> matmul(std::shared_ptr<WebNNOperand> a, std::shared_ptr<WebNNOperand> b) override;
  std::shared_ptr<WebNNOperand> relu(std::shared_ptr<WebNNOperand> input) override;
  std::shared_ptr<WebNNOperand> softmax(std::shared_ptr<WebNNOperand> input) override;
  
  std::shared_ptr<WebNNGraph> build(const std::map<std::string, std::shared_ptr<WebNNOperand>>& outputs) override;

  // Internal methods
  ggml_context* getGgmlContext() const { return ctx_; }
  ggml_backend_t getBackend() const { return backend_; }

private:
  WebNNContextOptions options_;
  ggml_context* ctx_;
  ggml_backend_t backend_;
  std::unordered_map<std::string, std::shared_ptr<GgmlWebNNOperand>> inputs_;
  
  ggml_type getGgmlType(const std::string& dataType) const;
  size_t getTypeSize(const std::string& dataType) const;
};

// GGML implementation of WebNN Operand
class GgmlWebNNOperand : public WebNNOperand {
public:
  GgmlWebNNOperand(const WebNNOperandDescriptor& descriptor, ggml_tensor* tensor);
  ~GgmlWebNNOperand() override = default;
  
  const WebNNOperandDescriptor& descriptor() const override { return descriptor_; }
  ggml_tensor* getTensor() const { return tensor_; }

private:
  WebNNOperandDescriptor descriptor_;
  ggml_tensor* tensor_;
};

// GGML implementation of WebNN Graph
class GgmlWebNNGraph : public WebNNGraph {
public:
  GgmlWebNNGraph(
    ggml_context* ctx,
    ggml_backend_t backend,
    const std::map<std::string, std::shared_ptr<WebNNOperand>>& outputs,
    const std::unordered_map<std::string, std::shared_ptr<GgmlWebNNOperand>>& inputs
  );
  ~GgmlWebNNGraph() override;
  
  std::shared_ptr<WebNNComputeResult> compute(const std::map<std::string, const void*>& inputs) override;

private:
  ggml_context* ctx_;
  ggml_backend_t backend_;
  ggml_cgraph* cgraph_;
  std::map<std::string, std::shared_ptr<WebNNOperand>> outputs_;
  std::unordered_map<std::string, std::shared_ptr<GgmlWebNNOperand>> inputs_;
};

// GGML implementation of WebNN Compute Result
class GgmlWebNNComputeResult : public WebNNComputeResult {
public:
  GgmlWebNNComputeResult(const std::map<std::string, std::shared_ptr<WebNNOperand>>& outputs);
  ~GgmlWebNNComputeResult() override = default;
  
  const void* getOutput(const std::string& name) const override;
  std::vector<std::string> getOutputNames() const override;

private:
  std::map<std::string, std::shared_ptr<WebNNOperand>> outputs_;
};

} // namespace nn
} // namespace transmute