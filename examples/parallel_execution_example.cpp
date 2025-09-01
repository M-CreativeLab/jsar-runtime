/**
 * Example demonstrating multi-threaded parallel execution in JSAR Runtime ECS
 * 
 * This example shows how to:
 * 1. Enable parallel execution for ECS systems
 * 2. Create systems that benefit from parallelization
 * 3. Configure parallel WebContent rendering
 */

#include <client/builtin_scene/ecs.hpp>
#include <client/builtin_scene/ecs-inl.hpp>
#include <iostream>
#include <chrono>
#include <thread>

using namespace builtin_scene::ecs;

// Example component for demonstration
class TransformComponent : public Component
{
public:
    TransformComponent(float x = 0, float y = 0, float z = 0) 
        : x(x), y(y), z(z) {}

    float x, y, z;
};

class VelocityComponent : public Component  
{
public:
    VelocityComponent(float vx = 0, float vy = 0, float vz = 0)
        : vx(vx), vy(vy), vz(vz) {}

    float vx, vy, vz;
};

// Example system that performs physics simulation
class PhysicsSystem : public System
{
public:
    const std::string name() const override 
    {
        return "PhysicsSystem";
    }

    void onExecute() override
    {
        auto entities = queryEntitiesWithComponent<TransformComponent>();
        
        std::cout << "PhysicsSystem processing " << entities.size() << " entities..." << std::endl;
        
        // Simulate physics computation
        for (auto& [entityId, transform] : entities)
        {
            auto velocity = getComponent<VelocityComponent>(entityId);
            if (velocity)
            {
                // Update position based on velocity
                transform->x += velocity->vx * 0.016f; // 60fps delta
                transform->y += velocity->vy * 0.016f;
                transform->z += velocity->vz * 0.016f;
            }
        }
        
        // Simulate heavy computation
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        std::cout << "PhysicsSystem completed" << std::endl;
    }
};

// Example system that performs animation updates
class AnimationSystem : public System
{
public:
    const std::string name() const override
    {
        return "AnimationSystem";
    }

    void onExecute() override
    {
        auto entities = queryEntitiesWithComponent<TransformComponent>();
        
        std::cout << "AnimationSystem processing " << entities.size() << " entities..." << std::endl;
        
        // Simulate animation computation
        for (auto& [entityId, transform] : entities)
        {
            // Apply some animation logic (e.g., rotation, scaling)
            // This could be skeletal animation, tweening, etc.
        }
        
        // Simulate heavy computation
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        
        std::cout << "AnimationSystem completed" << std::endl;
    }
};

// Example system that performs audio processing
class AudioSystem : public System
{
public:
    const std::string name() const override
    {
        return "AudioSystem";
    }

    void onExecute() override
    {
        std::cout << "AudioSystem processing audio..." << std::endl;
        
        // Simulate audio processing (3D positioning, effects, etc.)
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
        
        std::cout << "AudioSystem completed" << std::endl;
    }
};

// Example application that demonstrates parallel execution
class ParallelApp : public App
{
public:
    void setup()
    {
        std::cout << "Setting up Parallel Execution Example..." << std::endl;
        
        // Register components
        registerComponent<TransformComponent>();
        registerComponent<VelocityComponent>();
        
        // Create some test entities
        for (int i = 0; i < 100; ++i)
        {
            spawn(
                TransformComponent(i * 1.0f, i * 0.5f, 0),
                VelocityComponent(0.1f, -0.05f, 0)
            );
        }
        
        // Create systems
        auto physicsSystem = System::Make<PhysicsSystem>();
        auto animationSystem = System::Make<AnimationSystem>();
        auto audioSystem = System::Make<AudioSystem>();
        
        // Add systems to the same scheduler label - they will run in parallel when enabled
        addSystem(SchedulerLabel::kUpdate, physicsSystem);
        addSystem(SchedulerLabel::kUpdate, animationSystem);
        addSystem(SchedulerLabel::kUpdate, audioSystem);
        
        std::cout << "Systems added to ECS" << std::endl;
    }
    
    void runSequential()
    {
        std::cout << "\n=== Running with SEQUENTIAL execution ===" << std::endl;
        setParallelSystemsEnabled(false);
        
        auto start = std::chrono::high_resolution_clock::now();
        update(); // This will run all systems sequentially
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Sequential execution took: " << duration.count() << "ms" << std::endl;
    }
    
    void runParallel()
    {
        std::cout << "\n=== Running with PARALLEL execution ===" << std::endl;
        setParallelSystemsEnabled(true);
        
        auto start = std::chrono::high_resolution_clock::now();
        update(); // This will run systems in parallel
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Parallel execution took: " << duration.count() << "ms" << std::endl;
    }
};

int main()
{
    std::cout << "JSAR Runtime Parallel Execution Example" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    auto app = std::make_shared<ParallelApp>();
    app->setup();
    
    // Demonstrate the difference between sequential and parallel execution
    app->runSequential();
    app->runParallel();
    
    // Show configuration options
    std::cout << "\n=== Configuration Examples ===" << std::endl;
    std::cout << "Environment Variables:" << std::endl;
    std::cout << "  export JSAR_PARALLEL_SYSTEMS=1" << std::endl;
    std::cout << "  export JSAR_PARALLEL_RENDER=true" << std::endl;
    
    std::cout << "\nRuntime API:" << std::endl;
    std::cout << "  app->setParallelSystemsEnabled(true);" << std::endl;
    std::cout << "  app->setParallelRenderEnabled(true);" << std::endl;
    
    std::cout << "\nCurrent Settings:" << std::endl;
    std::cout << "  Parallel Systems: " << (app->isParallelSystemsEnabled() ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Parallel Render: " << (app->isParallelRenderEnabled() ? "ENABLED" : "DISABLED") << std::endl;
    
    return 0;
}