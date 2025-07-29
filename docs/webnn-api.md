# WebNN API Implementation

## Overview

This implementation provides a comprehensive WebNN (Web Neural Network) API for JSAR, enabling developers to build and execute machine learning models directly in the spatial web environment.

## Features

### Supported Operations

#### Basic Math Operations
- `add()` - Element-wise addition
- `sub()` - Element-wise subtraction  
- `mul()` - Element-wise multiplication
- `div()` - Element-wise division
- `matmul()` - Matrix multiplication

#### Activation Functions
- `relu()` - Rectified Linear Unit
- `sigmoid()` - Sigmoid activation
- `tanh()` - Hyperbolic tangent
- `softmax()` - Softmax normalization

#### Convolution Operations
- `conv2d()` - 2D convolution
- `averagePool2d()` - 2D average pooling
- `maxPool2d()` - 2D max pooling
- `globalAveragePool2d()` - Global average pooling

#### Shape Operations
- `reshape()` - Tensor reshaping
- `transpose()` - Tensor transposition
- `reduceMean()` - Mean reduction along axes

#### Normalization
- `batchNormalization()` - Batch normalization

#### Element-wise Functions
- `pow()` - Power function
- `sqrt()` - Square root

### Device Support
- **CPU**: Full support with optimized computation
- **GPU**: Planned for future releases

### Data Types
- `float32` - 32-bit floating point (primary)
- `float16` - 16-bit floating point
- `int32` - 32-bit signed integer
- `uint32` - 32-bit unsigned integer
- `int8` - 8-bit signed integer
- `uint8` - 8-bit unsigned integer

## Usage Examples

### Basic Graph Building

```javascript
// Create ML context
const context = await navigator.ml.createContext({
  deviceType: 'cpu'
});

// Create graph builder
const builder = context.createGraphBuilder();

// Define inputs
const input = builder.input('data', {
  type: 'float32',
  dimensions: [1, 784] // MNIST image flattened
});

// Build a simple neural network
const weights1 = builder.constant({
  type: 'float32',
  dimensions: [784, 128]
}, weightsData1);

const bias1 = builder.constant({
  type: 'float32', 
  dimensions: [128]
}, biasData1);

const hidden = builder.add(builder.matmul(input, weights1), bias1);
const activated = builder.relu(hidden);

const weights2 = builder.constant({
  type: 'float32',
  dimensions: [128, 10]
}, weightsData2);

const output = builder.matmul(activated, weights2);
const prediction = builder.softmax(output);

// Build the graph
const graph = await builder.build({ prediction });
```

### Executing the Graph

```javascript
// Prepare input data
const inputData = new Float32Array(784);
// ... fill with image data

// Prepare output buffer
const outputData = new Float32Array(10);

// Execute
const result = await graph.compute(
  { data: inputData },
  { prediction: outputData }
);

console.log('Prediction:', result.outputs.prediction);
```

### Convolutional Neural Network

```javascript
const builder = context.createGraphBuilder();

// Input: batch_size=1, channels=3, height=32, width=32
const input = builder.input('image', {
  type: 'float32',
  dimensions: [1, 3, 32, 32]
});

// Convolution layer
const filter = builder.constant({
  type: 'float32',
  dimensions: [16, 3, 3, 3] // 16 filters, 3 input channels, 3x3 kernel
}, filterWeights);

const conv = builder.conv2d(input, filter, {
  padding: [1, 1, 1, 1],
  strides: [1, 1]
});

const pooled = builder.maxPool2d(conv, {
  windowDimensions: [2, 2],
  strides: [2, 2]
});

const activated = builder.relu(pooled);

// Build and execute
const graph = await builder.build({ output: activated });
```

## Architecture

### TypeScript Layer (`lib/webnn.ts`)
- Implements W3C WebNN specification
- Provides type-safe JavaScript API
- Handles shape inference and validation
- Manages asynchronous operations

### Native Backend (`src/bindings/webnn/`)
- C++ backend for ML operations
- Integration layer for ggml
- Memory management and optimization
- Platform-specific acceleration

### Integration Points
- **Navigator API**: `navigator.ml` provides access to WebNN
- **Worker Support**: Can be used in Web Workers
- **Memory Management**: Efficient tensor memory handling
- **Error Handling**: Comprehensive error reporting

## Future Enhancements

### Short Term
- [ ] Complete ggml integration
- [ ] Add more operators (LSTM, GRU, attention)
- [ ] Implement quantization support
- [ ] Add model format loaders (ONNX, TensorFlow Lite)

### Long Term
- [ ] GPU acceleration via WebGPU/Metal/Vulkan
- [ ] Distributed inference
- [ ] Model optimization and pruning
- [ ] Hardware-specific optimizations

## Testing

Run the WebNN test suite:

```bash
npm run test
```

Test in browser with the provided HTML test page:
```
fixtures/html/webnn-test.html
```

## Compatibility

- **WebNN Specification**: Follows W3C WebNN Working Draft
- **Platform Support**: Desktop and mobile platforms
- **Integration**: Compatible with existing JSAR APIs
- **Performance**: Optimized for real-time applications

## API Reference

For complete API documentation, see the TypeScript definitions in `lib/webnn.ts` and test examples in `tests/webnn.test.ts`.