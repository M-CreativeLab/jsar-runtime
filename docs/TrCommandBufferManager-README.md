# TrCommandBufferManager Implementation

## Overview

`TrCommandBufferManager` is a renderer-side class that manages the encoding of command buffers for the JSAR Runtime. It accepts `TrCommandBufferBase` instances from the web/content process, encodes them into WebGPU command buffers, and provides encoded passes for submission by the host renderer.

## Purpose

The JSAR runtime uses a multi-process architecture where rendering commands flow from the web/content process to the renderer. The manager serves as the bridge between these serialized commands and the WebGPU-based rendering system.

### Why WebGPU Command Buffers?

WebGPU command buffers offer several advantages:
1. **Pre-recording**: Commands can be recorded ahead of time
2. **Reusability**: Same command buffer can be submitted multiple times
3. **Flexibility**: Host can schedule rendering more flexibly
4. **Performance**: Static scenes can reuse encoded commands across frames

## Architecture

```
Web/Content Process              Renderer Process                   GPU
─────────────────               ──────────────                    ─────
TrCommandBufferBase  ──IPC──>  TrCommandBufferManager  ──>  GPUCommandBuffer  ──>  GPU
  (GLES-style)                  (Type Dispatch)                 (WebGPU)          (Execution)
                                       │
                                       ├─> Framebuffer Commands -> Render Pass
                                       ├─> Resource Commands    -> Resource Creation
                                       └─> Generic Commands     -> Copy/Query Operations
```

## Key Features

### Thread-Safe Operation
- All public methods protected by `std::mutex`
- Safe concurrent command addition from multiple threads
- Lock-free read-only access to encoded passes (const reference)

### Type-Based Dispatch
Commands are classified using `CommandTypes` helpers:
- **Framebuffer-Dependent**: Draw calls, clears, viewport changes → Render passes
- **Resource-Creating**: Buffer/texture/shader creation → Resource operations
- **XR Control**: Frame lifecycle management → Handled by XR system
- **Generic**: Copy operations, queries → Copy/compute passes

### XR Support
`EncodedPass` includes `TrXRFrameRenderingInfo` with:
- `sessionId`: XR session identifier
- `stereoId`: Stereo rendering identifier
- `viewIndex`: Eye/view index (0=left, 1=right)

This enables the host to route passes to correct XR render targets.

## API

### Construction
```cpp
TrCommandBufferManager(GPUDeviceBase* device);
```
Requires a non-null GPU device. Throws `std::invalid_argument` if device is null.

### Adding Commands
```cpp
void addCommandBuffer(std::unique_ptr<TrCommandBufferBase> cmd);
```
- Thread-safe
- Classifies command type
- Dispatches to appropriate encoding path
- Enqueues encoded pass for later submission

### Retrieving Passes
```cpp
const std::vector<EncodedPass>& getEncodedPasses() const;
size_t getEncodedPassCount() const;
```
- Thread-safe (read lock)
- Returns const reference to avoid copying
- Valid until `clearEncodedPasses()` called

### Clearing Passes
```cpp
void clearEncodedPasses();
```
- Thread-safe (write lock)
- Should be called after GPU submission completes
- Releases GPU resources

## Usage Example

```cpp
// Initialize
auto manager = std::make_unique<TrCommandBufferManager>(device);

// Add commands (can be from multiple threads)
manager->addCommandBuffer(std::move(cmd1));
manager->addCommandBuffer(std::move(cmd2));

// In render loop
const auto& passes = manager->getEncodedPasses();
for (const auto& pass : passes) {
  if (pass.isXRPass) {
    host->submitToXRView(pass.renderingInfo.viewIndex, 
                         pass.commandBuffer.get());
  } else {
    host->submitToMainRenderPass(pass.commandBuffer.get());
  }
}

// After submission completes
manager->clearEncodedPasses();
```

See `docs/examples/command_buffer_manager_usage.cpp` for more examples.

## Implementation Status

### ✅ Completed
- Complete API surface
- Thread-safe storage and access
- Type dispatch logic
- Unit tests
- Documentation

### ⏳ In Progress (Marked with TODO/FIXME)
The actual encoding implementation is deferred because the WebGPU layer in JSAR is incomplete. Each encoding method (`encodeFramebufferCommand`, `encodeResourceCommand`, `encodeGenericCommand`) contains:

1. **TODO comments** describing the implementation steps
2. **WebGPU spec references** for the required functionality
3. **FIXME markers** identifying missing WebGPU APIs

Example from `encodeFramebufferCommand`:
```cpp
// TODO: Implement framebuffer command encoding
// WebGPU Specification Reference:
// https://www.w3.org/TR/webgpu/#render-passes
//
// Implementation steps:
// 1. Create GPUCommandEncoder
// 2. Begin render pass
// 3. Encode commands
// 4. Finish encoder
// ...
```

### 🔴 Missing WebGPU Functionality

The WebGPU implementation gaps are documented in `docs/webgpu-implementation-status.md`:

**Priority 1 (Critical):**
- Copy operations in `GPUCommandEncoder`
- `setBindGroup()` in `GPURenderPassEncoder`
- Public texture creation APIs

**Priority 2 (Important):**
- Queue submission and completion tracking
- Buffer/texture data upload paths
- Pass descriptor validation

**Priority 3 (Nice to have):**
- Compute pass encoding
- Indirect drawing
- Query sets

## WebGPU Specification References

The implementation aligns with these WebGPU spec sections:

- **Command Encoding**: https://www.w3.org/TR/webgpu/#command-encoding
- **Command Buffers**: https://www.w3.org/TR/webgpu/#command-buffers
- **Render Passes**: https://www.w3.org/TR/webgpu/#render-passes
- **Resource Creation**: https://www.w3.org/TR/webgpu/#resource-creation
- **Synchronization**: https://www.w3.org/TR/webgpu/#synchronization

All code comments reference specific sections when describing missing functionality.

## Testing

### Unit Tests (`tests/command_buffer_manager.cpp`)

**API Surface Tests:**
- Construction validation (null device throws)
- Basic operations (add, get, clear, count)
- Command classification correctness

**Thread Safety Tests:**
- Concurrent command addition
- Concurrent reads and writes
- No data races or deadlocks

**Integration Tests:**
- Deferred until WebGPU implementation complete
- Will test actual encoding when APIs available

### Test Strategy
Tests focus on the manager's API and logic rather than GPU operations, since:
1. GPU device mocking is complex
2. Encoding implementation is deferred
3. API surface and thread safety can be validated independently

## Future Work

### Short-term
1. Complete WebGPU API gaps (see `docs/webgpu-implementation-status.md`)
2. Implement GLES-to-WebGPU translation layer
3. Add actual encoding logic to stub methods
4. Integrate with host renderer submission system

### Long-term
1. Optimize pass batching for reduced submission overhead
2. Add command buffer reuse for static scenes
3. Implement queue completion callbacks for resource lifecycle
4. Profile and optimize encoding performance

## Design Decisions

### Why not encode immediately?
Encoding is deferred to allow batching and flexible scheduling. The manager acts as a staging area where commands accumulate before being encoded into passes.

### Why separate pass types?
Different command types have different encoding requirements:
- Framebuffer commands need render passes
- Resource commands may execute synchronously
- Generic commands use copy/compute passes

Separate paths allow specialized handling for each type.

### Why store encoded passes?
Storing allows:
- Inspection before submission
- Routing to different render targets
- Reuse across frames (future optimization)

### Why thread-safe?
Commands may arrive from multiple sources (IPC threads, worker threads). Thread safety ensures correct operation regardless of call pattern.

## Related Files

- **Implementation**: `src/renderer/command_buffer_manager.{hpp,cpp}`
- **Tests**: `tests/command_buffer_manager.cpp`
- **Documentation**: 
  - This file: `docs/TrCommandBufferManager-README.md`
  - Status: `docs/webgpu-implementation-status.md`
  - Examples: `docs/examples/command_buffer_manager_usage.cpp`
- **Dependencies**: 
  - `src/common/command_buffers/base.hpp` - Command buffer types
  - `src/common/command_buffers/shared.hpp` - CommandTypes helpers
  - `src/common/command_buffers/gpu/gpu_device.hpp` - GPU device interface
  - `src/common/xr/types.hpp` - XR rendering info

## Contributing

When implementing encoding functionality:

1. Read the WebGPU spec section referenced in TODO comments
2. Implement according to spec semantics
3. Update or remove TODO/FIXME markers
4. Add integration tests for the implemented path
5. Update `docs/webgpu-implementation-status.md`

For questions, refer to:
- WebGPU specification: https://www.w3.org/TR/webgpu/
- This documentation
- Inline code comments with spec references
