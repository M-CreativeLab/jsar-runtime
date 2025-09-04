# XR Rendering Pipeline Refactor

This document describes the refactored XR rendering pipeline that maintains GPURenderPassEncoder queues per StereoRenderingFrame.

## Overview

The XR rendering pipeline has been refactored to provide fine-grained control over multiple render passes per frame and prepare the architecture for advanced effects and multi-pass composition.

## Key Components

### 1. StereoRenderingFrame Enhancement

The `xr::StereoRenderingFrame` class now maintains a vector of `GPURenderPassEncoder` instances:

```cpp
class StereoRenderingFrame {
    // ... existing members ...
    
    vector<commandbuffers::GPURenderPassEncoder> m_RenderPassEncoders;
    
public:
    // Accessor methods
    vector<commandbuffers::GPURenderPassEncoder>& getRenderPassEncoders();
    void addRenderPassEncoder(commandbuffers::GPURenderPassEncoder encoder);
    void clearRenderPassEncoders();
};
```

### 2. XRRenderPassQueueBuilder

A new class `xr::XRRenderPassQueueBuilder` is responsible for:
- Collecting `TrCommandBufferRequest` objects from multiple sources
- Generating and maintaining an ordered queue/vector of `GPURenderPassEncoder` objects
- Supporting efficient queue updates with thread safety

```cpp
class XRRenderPassQueueBuilder {
public:
    void addCommandBufferRequest(commandbuffers::TrCommandBufferBase* request);
    std::vector<commandbuffers::GPURenderPassEncoder> buildRenderPassQueue();
    void clear();
    size_t getRequestCount() const;
    bool hasRequests() const;
};
```

### 3. Integration with TrContentRenderer

The `TrContentRenderer::onCommandBufferRequestReceived()` method has been refactored to:
- Use the new `XRRenderPassQueueBuilder` for processing command buffer requests
- Build and maintain GPURenderPassEncoder queues for each frame
- Ensure proper resource management and thread safety

## Usage Example

```cpp
// In TrContentRenderer::onCommandBufferRequestReceived()
frame->addCommandBuffer(req, viewIndex);

// Process the command buffer request into render pass encoders
xrRenderPassQueueBuilder->addCommandBufferRequest(req);
auto renderPassEncoders = xrRenderPassQueueBuilder->buildRenderPassQueue();

// Add render pass encoders to the frame
for (auto& encoder : renderPassEncoders) {
    frame->addRenderPassEncoder(std::move(encoder));
}

// Access render pass encoders later during rendering
auto& encoders = frame->getRenderPassEncoders();
for (auto& encoder : encoders) {
    // Execute render pass
}
```

## Benefits

1. **Fine-grained Control**: Multiple render passes per frame for advanced rendering techniques
2. **Modularity**: Clear separation between command buffer processing and render pass management
3. **Extensibility**: Easy to add new render pass types and effects
4. **Performance**: Optimized queue management and resource handling
5. **Thread Safety**: Concurrent access support for multi-threaded rendering

## Future Enhancements

This architecture enables future enhancements such as:
- Multi-pass effects (shadow mapping, reflection, post-processing)
- Advanced compositing techniques
- Better pipeline diagnostics and debugging
- Dynamic render pass optimization

## Backward Compatibility

The refactor maintains full backward compatibility with existing code. All existing functionality continues to work as before, with the new render pass encoder system working alongside the traditional command buffer system.