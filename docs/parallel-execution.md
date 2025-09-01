# Multi-threaded Parallel Execution

JSAR Runtime supports configurable multi-threaded parallel execution for ECS systems and WebContent rendering to improve performance in complex spatial applications.

## Overview

The parallel execution feature allows:
- **ECS Systems**: Multiple systems within the same scheduler label to run concurrently
- **WebContent Rendering**: Multiple WebContent entities to be rendered in parallel

This can significantly improve performance on multi-core systems, especially for scenes with many dynamic elements or complex rendering operations.

## Configuration

### Environment Variables

Set these environment variables before starting the application:

```bash
# Enable parallel ECS system execution
export JSAR_PARALLEL_SYSTEMS=1

# Enable parallel WebContent rendering  
export JSAR_PARALLEL_RENDER=true
```

### Runtime API

Configure parallel execution programmatically:

```cpp
#include <client/builtin_scene/ecs.hpp>

auto app = std::make_shared<builtin_scene::ecs::App>();

// Enable parallel system execution
app->setParallelSystemsEnabled(true);

// Enable parallel render execution
app->setParallelRenderEnabled(true);

// Query current settings
bool systemsParallel = app->isParallelSystemsEnabled();
bool renderParallel = app->isParallelRenderEnabled();
```

## Behavior

### Default Behavior
- **Sequential Execution**: Both ECS systems and WebContent rendering execute sequentially by default
- **Backwards Compatibility**: Existing applications continue to work without changes
- **Opt-in**: Parallel execution must be explicitly enabled

### ECS Systems Parallelization

When `JSAR_PARALLEL_SYSTEMS=1` or `setParallelSystemsEnabled(true)`:

- Systems within the same `SchedulerLabel` run concurrently using `std::async`
- Systems in different scheduler labels still run in sequence (e.g., `kPreUpdate` before `kUpdate`)
- Single systems continue to run sequentially (no overhead)
- Thread safety is maintained through existing ECS mutex infrastructure

```cpp
// These systems will run in parallel when enabled:
app->addSystem(SchedulerLabel::kUpdate, physicsSystem);
app->addSystem(SchedulerLabel::kUpdate, animationSystem);
app->addSystem(SchedulerLabel::kUpdate, audioSystem);
```

### WebContent Rendering Parallelization

When `JSAR_PARALLEL_RENDER=1` or `setParallelRenderEnabled(true)`:

- Multiple `WebContent` entities render concurrently
- Each entity's `render()` method executes in a separate thread
- Surface dirty flags are updated safely after rendering completes
- Single entities continue to render sequentially (no overhead)

## Performance Considerations

### When to Enable Parallel Execution

**Enable for:**
- Applications with multiple heavy systems (physics, animation, complex rendering)
- Scenes with many WebContent entities (spatial web browsers, UI panels)  
- Multi-core target platforms (desktop, high-end mobile)
- CPU-bound workloads that can benefit from parallelization

**Keep Disabled for:**
- Simple applications with few systems/entities
- Single-core or resource-constrained platforms
- GPU-bound rendering workloads
- Applications with heavy system interdependencies

### Performance Characteristics

| Workload Type | Systems Count | Expected Speedup | Overhead |
|---------------|---------------|------------------|----------|
| CPU-bound systems | 1 | 1x (no change) | None |
| CPU-bound systems | 2-4 | 1.5-3x | Low |  
| CPU-bound systems | 4+ | 2-4x | Low |
| GPU-bound rendering | Any | 1x (limited) | Low |
| I/O-bound operations | 2+ | 1.2-2x | Low |

### Thread Safety

The implementation maintains thread safety through:

- **Existing Mutexes**: Leverages `mutexForEntities_` and `mutexForSystems_` in ECS App
- **Immutable Data**: Systems operate on independent entity/component data  
- **Synchronization**: Uses `std::future::wait()` to ensure completion before proceeding
- **Atomic Operations**: Safe concurrent access patterns where needed

## Limitations

### ECS Systems
- Systems with shared mutable state may require additional synchronization
- Chain systems (`system1->chain(system2)`) still execute sequentially within their chain
- Heavy inter-system dependencies may limit parallelization benefits

### WebContent Rendering
- OpenGL/Vulkan context sharing limitations may affect some rendering operations
- Very fast rendering operations may not benefit significantly from parallelization
- Memory bandwidth may become a bottleneck with many concurrent rendering tasks

## Troubleshooting

### Common Issues

**Systems not running in parallel:**
- Verify parallel execution is enabled: `app->isParallelSystemsEnabled()`
- Check that multiple systems exist in the same scheduler label
- Ensure systems are not chained (chained systems run sequentially)

**Performance degradation:**
- Disable parallel execution for GPU-bound workloads
- Consider system dependencies that may cause contention
- Profile to identify actual bottlenecks

**Thread safety issues:**
- Review custom systems for shared mutable state
- Add proper synchronization for cross-system communication
- Use ECS component queries instead of direct shared variables

### Debugging

Enable ECS time profiling to measure system execution times:

```cpp
// Uncomment in ecs.hpp
#define TR_ECS_ENABLE_TIME_PROFILING
```

This will output execution times for each system, helping identify parallelization candidates.

## Example Usage

```cpp
#include <client/builtin_scene/ecs.hpp>

class MyApp : public builtin_scene::ecs::App {
public:
    void setup() {
        // Enable parallel execution
        setParallelSystemsEnabled(true);
        setParallelRenderEnabled(true);
        
        // Add multiple systems that can run in parallel
        addSystem(SchedulerLabel::kUpdate, std::make_shared<PhysicsSystem>());
        addSystem(SchedulerLabel::kUpdate, std::make_shared<AnimationSystem>());
        addSystem(SchedulerLabel::kUpdate, std::make_shared<AudioSystem>());
        
        // These will run in parallel when kUpdate label executes
    }
};
```

This documentation provides guidance for using the parallel execution features effectively while maintaining application stability and performance.