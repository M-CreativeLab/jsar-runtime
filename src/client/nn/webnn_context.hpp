#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace transmute {
namespace nn {

// Forward declarations
class WebNNOperand;
class WebNNGraph;
class WebNNComputeResult;

// WebNN Device Type enumeration
enum class WebNNDeviceType {
  CPU,
  GPU,
  NPU
};

// WebNN Power Preference enumeration
enum class WebNNPowerPreference {
  Default,
  HighPerformance,
  LowPower
};

// WebNN Context Options
struct WebNNContextOptions {
  WebNNDeviceType deviceType = WebNNDeviceType::CPU;
  WebNNPowerPreference powerPreference = WebNNPowerPreference::Default;
};

// WebNN Operand Descriptor
struct WebNNOperandDescriptor {
  std::string dataType;
  std::vector<int32_t> dimensions;
};

// WebNN Context - Main interface for WebNN operations
class WebNNContext {
public:
  static std::shared_ptr<WebNNContext> Create(const WebNNContextOptions& options = {});
  
  virtual ~WebNNContext() = default;
  
  // Operand creation
  virtual std::shared_ptr<WebNNOperand> input(const std::string& name, const WebNNOperandDescriptor& descriptor) = 0;
  virtual std::shared_ptr<WebNNOperand> constant(const WebNNOperandDescriptor& descriptor, const void* data) = 0;
  
  // Operations
  virtual std::shared_ptr<WebNNOperand> add(std::shared_ptr<WebNNOperand> a, std::shared_ptr<WebNNOperand> b) = 0;
  virtual std::shared_ptr<WebNNOperand> mul(std::shared_ptr<WebNNOperand> a, std::shared_ptr<WebNNOperand> b) = 0;
  virtual std::shared_ptr<WebNNOperand> matmul(std::shared_ptr<WebNNOperand> a, std::shared_ptr<WebNNOperand> b) = 0;
  virtual std::shared_ptr<WebNNOperand> relu(std::shared_ptr<WebNNOperand> input) = 0;
  virtual std::shared_ptr<WebNNOperand> softmax(std::shared_ptr<WebNNOperand> input) = 0;
  
  // Graph compilation
  virtual std::shared_ptr<WebNNGraph> build(const std::map<std::string, std::shared_ptr<WebNNOperand>>& outputs) = 0;

protected:
  WebNNContext() = default;
};

// WebNN Operand - Represents a tensor operand
class WebNNOperand {
public:
  virtual ~WebNNOperand() = default;
  virtual const WebNNOperandDescriptor& descriptor() const = 0;

protected:
  WebNNOperand() = default;
};

// WebNN Graph - Compiled computational graph
class WebNNGraph {
public:
  virtual ~WebNNGraph() = default;
  
  // Compute execution
  virtual std::shared_ptr<WebNNComputeResult> compute(const std::map<std::string, const void*>& inputs) = 0;

protected:
  WebNNGraph() = default;
};

// WebNN Compute Result - Result of graph execution
class WebNNComputeResult {
public:
  virtual ~WebNNComputeResult() = default;
  virtual const void* getOutput(const std::string& name) const = 0;
  virtual std::vector<std::string> getOutputNames() const = 0;

protected:
  WebNNComputeResult() = default;
};

} // namespace nn
} // namespace transmute