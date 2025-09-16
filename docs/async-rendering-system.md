# Async Rendering Scheduling System

## Overview

The async rendering scheduling system enables non-blocking text rendering and SDF texture generation in JSAR Runtime. This system addresses performance bottlenecks where complex text rendering could cause frame drops or UI stalls by offloading heavy operations to background threads.

## Architecture

### Core Components

1. **WebContent** - Enhanced with thread-safe async state tracking
2. **RenderContentBaseSystem** - Provides generic async scheduling infrastructure  
3. **RenderTextSystem** - Refactored to use async rendering for heavy text operations
4. **UpdateTextureSystem** - Updated to handle async completion safely

### Thread Safety Design

The system uses a multi-layered approach to ensure thread safety:

- **Mutex Protection**: All async state updates use `std::mutex async_state_mutex_`
- **Atomic State Changes**: Surface dirty flags are updated within lock scope
- **Race Condition Prevention**: Systems coordinate through shared state flags
- **Exception Safety**: Async operations handle failures gracefully

## API Reference

### WebContent Async State Management

```cpp
// Check if async rendering is currently in progress (thread-safe)
bool isAsyncRenderingInProgress() const;

// Set async rendering state (thread-safe)
void setAsyncRenderingInProgress(bool inProgress);

// Mark async rendering as completed (thread-safe)
// Sets surface dirty and clears async flag atomically
void markAsyncRenderingCompleted();
```

### RenderContentBaseSystem Async Scheduling

```cpp
// Schedule heavy rendering work to run asynchronously
void scheduleAsyncSurfaceUpdate(
    ecs::EntityId entity, 
    WebContent &content,
    std::function<bool(ecs::EntityId, WebContent &)> asyncRenderFunc
);
```

**Parameters:**
- `entity`: The entity ID to render
- `content`: The WebContent to render (passed by reference for thread safety)
- `asyncRenderFunc`: Function containing the rendering logic to execute asynchronously

**Thread Safety Guarantees:**
- Method is thread-safe and can be called from any thread
- The `asyncRenderFunc` will be executed on a separate worker thread
- Automatic cleanup of completed async operations
- Exception handling prevents worker thread failures from affecting main thread

### RenderTextSystem Enhanced Methods

```cpp
// Determine if async rendering should be used
bool shouldUseAsyncRendering(const std::string &text, const WebContent &content);

// Synchronous text rendering (for simple text)
bool renderTextSync(ecs::EntityId entity, WebContent &content);

// Asynchronous text rendering (for complex text with SDF generation)
bool renderTextAsync(ecs::EntityId entity, WebContent &content);
```

## Usage Examples

### Basic Async Scheduling

```cpp
// In a custom rendering system
class MyRenderSystem : public RenderContentBaseSystem {
    bool render(ecs::EntityId entity, WebContent &content) override {
        if (isComplexRenderingNeeded(content)) {
            // Schedule async rendering
            scheduleAsyncSurfaceUpdate(entity, content, 
                [this](ecs::EntityId asyncEntity, WebContent &asyncContent) -> bool {
                    return performComplexRendering(asyncEntity, asyncContent);
                });
            return false; // Surface not immediately ready
        } else {
            // Render synchronously for simple cases
            return performSimpleRendering(entity, content);
        }
    }
};
```

### Text Rendering Flow

```cpp
// Automatic async/sync decision in RenderTextSystem
bool RenderTextSystem::render(ecs::EntityId entity, WebContent &content) {
    auto textComponent = getComponent<Text2d>(entity);
    string &text = textComponent->content;
    
    if (shouldUseAsyncRendering(text, content)) {
        // Large/complex text: use async rendering
        scheduleAsyncSurfaceUpdate(entity, content, 
            [this](ecs::EntityId asyncEntity, WebContent &asyncContent) -> bool {
                return renderTextAsync(asyncEntity, asyncContent);
            });
        return false;
    } else {
        // Simple text: render synchronously
        return renderTextSync(entity, content);
    }
}
```

## Performance Characteristics

### Async Rendering Triggers

The system uses intelligent heuristics to determine when async rendering should be used:

1. **Text Length**: Text content > 100 characters
2. **Content Area**: Rendering area > 10,000 pixels (100x100)
3. **SDF Generation**: Expected to be computationally expensive

### Performance Benefits

- **Main Thread Responsiveness**: Heavy operations don't block the render loop
- **Parallel Processing**: Multiple text elements can be processed simultaneously
- **Resource Efficiency**: Automatic cleanup prevents memory leaks
- **Selective Optimization**: Only complex content uses async path

## Implementation Details

### State Management Flow

1. **Initiation**: `RenderTextSystem` calls `scheduleAsyncSurfaceUpdate()`
2. **State Update**: Content marked as async rendering in progress
3. **Background Processing**: Worker thread executes rendering logic
4. **Completion**: `markAsyncRenderingCompleted()` sets surface dirty atomically
5. **Texture Update**: `UpdateTextureSystem` processes completed work

### Thread Coordination

```cpp
// UpdateTextureSystem selection criteria
auto selectContents = [](const WebContent &content) -> bool {
    return content.canvas() != nullptr && 
           content.isSurfaceDirty() && 
           !content.isAsyncRenderingInProgress(); // Skip async in-progress
};
```

### Error Handling

- **Exception Safety**: Async operations catch exceptions and restore state
- **Graceful Degradation**: Failed async operations fall back to sync rendering
- **Resource Cleanup**: Completed operations are automatically cleaned up

## Testing

The implementation includes comprehensive tests covering:

- **Thread Safety**: Concurrent access to async state
- **API Functionality**: Async scheduling and completion
- **Heuristics Validation**: Async vs sync decision making
- **Integration**: UpdateTextureSystem coordination

### Example Test

```cpp
TEST_CASE("Async state thread safety", "[async_rendering][thread_safety]") {
    // Multiple threads concurrently accessing async state
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&]() {
            content->setAsyncRenderingInProgress(true);
            REQUIRE(content->isAsyncRenderingInProgress());
            content->markAsyncRenderingCompleted();
            REQUIRE_FALSE(content->isAsyncRenderingInProgress());
        });
    }
    // All threads complete without race conditions
}
```

## Migration Guide

### For Existing Code

The async rendering system is backward compatible:

- **Existing synchronous workflows continue unchanged**
- **No breaking API changes to public interfaces**
- **Optional async functionality activated based on heuristics**

### For New Implementations

When creating custom rendering systems:

1. Inherit from `RenderContentBaseSystem`
2. Use `scheduleAsyncSurfaceUpdate()` for heavy operations
3. Implement proper error handling in async functions
4. Consider content complexity for async/sync decisions

## Best Practices

### When to Use Async Rendering

- **Heavy Computations**: SDF generation, complex layouts
- **Large Content**: Text > 100 characters or large rendering areas
- **Multiple Elements**: When processing many elements simultaneously

### When to Use Sync Rendering

- **Simple Content**: Small text elements, basic shapes
- **Real-time Requirements**: When immediate results are needed
- **Low Latency**: For interactive elements requiring instant feedback

### Thread Safety Guidelines

- **Always use provided async state methods** for coordination
- **Don't access WebContent members directly from worker threads** without proper synchronization
- **Handle exceptions in async functions** to prevent worker thread crashes
- **Use `markAsyncRenderingCompleted()`** to signal completion atomically

## Future Enhancements

### Potential Improvements

1. **Priority Queuing**: High-priority content processed first
2. **Resource Pooling**: Reuse worker threads for efficiency
3. **Progress Tracking**: Monitor async operation progress
4. **Cancellation Support**: Cancel pending operations when content changes

### Extension Points

The system is designed for extensibility:

- **Custom Heuristics**: Override `shouldUseAsyncRendering()` in subclasses
- **Additional Async Operations**: Use `scheduleAsyncSurfaceUpdate()` for other heavy tasks
- **Custom State Management**: Extend WebContent with operation-specific state

## Troubleshooting

### Common Issues

1. **Race Conditions**: Ensure proper use of async state methods
2. **Memory Leaks**: Automatic cleanup should prevent this, but monitor pending operations
3. **Performance Regression**: Check if async overhead exceeds benefits for simple content

### Debugging Tips

- **Enable Verbose Logging**: Monitor async operation lifecycle
- **Check Async State**: Use `isAsyncRenderingInProgress()` for debugging
- **Validate Thread Safety**: Use thread sanitizers during development
- **Monitor Resource Usage**: Track worker thread creation and cleanup