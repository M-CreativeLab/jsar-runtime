#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/graphics/webgpu_context.hpp>
#include <client/graphics/webgpu_command_encoder.hpp>
#include <client/graphics/webgpu_buffer.hpp>
#include <client/graphics/webgpu_texture.hpp>
#include <client/graphics/webgpu_bind_group.hpp>
#include <client/graphics/webgpu_render_pipeline.hpp>

using namespace client_graphics;

TEST_CASE("WebGPU Context Creation", "[webgpu-context]")
{
  SECTION("Create WebGPU context with default attributes")
  {
    WebGPUContextAttributes attrs;
    WebGPUContext context(attrs);
    
    REQUIRE_FALSE(context.isConfigured());
    REQUIRE(context.getConfiguration().powerPreference == "default");
    REQUIRE(context.getConfiguration().format == "bgra8unorm");
    REQUIRE(context.getConfiguration().colorSpace == "srgb");
    REQUIRE(context.getConfiguration().alphaMode == "opaque");
  }

  SECTION("Configure WebGPU context")
  {
    WebGPUContext context;
    WebGPUContextAttributes config;
    config.powerPreference = "high-performance";
    config.format = "rgba8unorm";
    
    context.configure(config);
    
    REQUIRE(context.isConfigured());
    REQUIRE(context.getConfiguration().powerPreference == "high-performance");
    REQUIRE(context.getConfiguration().format == "rgba8unorm");
  }
}

TEST_CASE("WebGPU Adapter Request", "[webgpu-adapter]")
{
  SECTION("Enumerate adapters")
  {
    WebGPUContext context;
    auto adapters = context.enumerateAdapters();
    
    REQUIRE_FALSE(adapters.empty());
    REQUIRE(adapters.size() >= 1);
    
    const auto& adapter = adapters[0];
    REQUIRE(adapter != nullptr);
    REQUIRE(adapter->info().vendor == "JSAR Runtime");
    REQUIRE(adapter->info().description == "Default WebGPU Adapter");
  }

  SECTION("Request adapter")
  {
    WebGPUContext context;
    auto adapter = context.requestAdapter();
    
    REQUIRE(adapter != nullptr);
    REQUIRE(adapter->info().vendor == "JSAR Runtime");
    
    // Check adapter capabilities
    const auto& features = adapter->features();
    const auto& limits = adapter->limits();
    
    REQUIRE(limits.maxTextureDimension2D() >= 8192);
    REQUIRE(limits.maxBindGroups() >= 4);
  }
}

TEST_CASE("WebGPU Device Creation", "[webgpu-device]")
{
  SECTION("Request device from adapter")
  {
    WebGPUContext context;
    auto adapter = context.requestAdapter();
    REQUIRE(adapter != nullptr);
    
    auto device = adapter->requestDevice("TestDevice");
    REQUIRE(device != nullptr);
    REQUIRE_FALSE(device->isLost());
    
    // Verify device properties
    REQUIRE(device->adapterInfo().vendor == "JSAR Runtime");
    REQUIRE(device->limits().maxTextureDimension2D() >= 8192);
  }

  SECTION("Device with required features")
  {
    WebGPUContext context;
    auto adapter = context.requestAdapter();
    REQUIRE(adapter != nullptr);
    
    // Request device with no specific features (should succeed)
    std::vector<std::string> requiredFeatures;
    auto device = adapter->requestDevice("TestDevice", requiredFeatures);
    REQUIRE(device != nullptr);
  }

  SECTION("Device error handling")
  {
    WebGPUContext context;
    auto adapter = context.requestAdapter();
    REQUIRE(adapter != nullptr);
    
    auto device = adapter->requestDevice("TestDevice");
    REQUIRE(device != nullptr);
    
    bool errorCalled = false;
    device->setUncapturedErrorCallback([&errorCalled](const std::string& type, const std::string& message) {
      errorCalled = true;
    });
    
    // Simulate device loss
    device->simulateLoss();
    REQUIRE(device->isLost());
    REQUIRE(errorCalled);
  }
}

TEST_CASE("WebGPU Command Recording", "[webgpu-commands]")
{
  SECTION("Create command encoder")
  {
    WebGPUContext context;
    auto adapter = context.requestAdapter();
    auto device = adapter->requestDevice("TestDevice");
    REQUIRE(device != nullptr);
    
    auto encoder = device->createCommandEncoder("TestEncoder");
    REQUIRE(encoder != nullptr);
  }

  SECTION("Create and finish command buffer")
  {
    WebGPUContext context;
    auto adapter = context.requestAdapter();
    auto device = adapter->requestDevice("TestDevice");
    auto encoder = device->createCommandEncoder("TestEncoder");
    
    auto commandBuffer = encoder->finish("TestCommandBuffer");
    REQUIRE(commandBuffer != nullptr);
    REQUIRE(commandBuffer->label() == "TestCommandBuffer");
    REQUIRE(commandBuffer->isEmpty()); // No commands recorded yet
  }

  SECTION("Record render pass commands")
  {
    WebGPUContext context;
    auto adapter = context.requestAdapter();
    auto device = adapter->requestDevice("TestDevice");
    auto encoder = device->createCommandEncoder("TestEncoder");
    
    // Create a simple render pass descriptor
    commandbuffers::GPURenderPassDescriptor descriptor;
    descriptor.label = "TestRenderPass";
    
    auto renderPass = encoder->beginRenderPass(descriptor);
    REQUIRE(renderPass != nullptr);
    REQUIRE_FALSE(renderPass->isEnded());
    
    // Record some drawing commands
    renderPass->draw(3, 1, 0, 0); // Draw a triangle
    renderPass->setViewport(0, 0, 800, 600);
    
    renderPass->end();
    REQUIRE(renderPass->isEnded());
    
    auto commandBuffer = encoder->finish("TestCommandBuffer");
    REQUIRE(commandBuffer != nullptr);
    REQUIRE_FALSE(commandBuffer->isEmpty()); // Should have commands now
    REQUIRE(commandBuffer->commandCount() >= 2); // At least draw and viewport commands
  }

  SECTION("Queue submission")
  {
    WebGPUContext context;
    auto adapter = context.requestAdapter();
    auto device = adapter->requestDevice("TestDevice");
    auto encoder = device->createCommandEncoder("TestEncoder");
    
    // Create simple command buffer
    commandbuffers::GPURenderPassDescriptor descriptor;
    auto renderPass = encoder->beginRenderPass(descriptor);
    renderPass->draw(3);
    renderPass->end();
    
    auto commandBuffer = encoder->finish();
    
    // Submit to queue
    std::vector<std::unique_ptr<WebGPUCommandBuffer>> commandBuffers;
    commandBuffers.push_back(std::move(commandBuffer));
    
    auto& queue = device->queue();
    REQUIRE_NOTHROW(queue.submit(commandBuffers));
  }

  SECTION("Command encoder error handling")
  {
    WebGPUContext context;
    auto adapter = context.requestAdapter();
    auto device = adapter->requestDevice("TestDevice");
    auto encoder = device->createCommandEncoder("TestEncoder");
    
    auto commandBuffer = encoder->finish();
    
    // Should throw when trying to use finished encoder
    REQUIRE_THROWS_AS(encoder->finish(), std::runtime_error);
    
    commandbuffers::GPURenderPassDescriptor descriptor;
    REQUIRE_THROWS_AS(encoder->beginRenderPass(descriptor), std::runtime_error);
  }
}

TEST_CASE("WebGPU Resource Placeholders", "[webgpu-resources]")
{
  SECTION("Create resource objects")
  {
    WebGPUBuffer buffer("TestBuffer");
    REQUIRE(buffer.label() == "TestBuffer");
    REQUIRE(buffer.id() != 0);
    
    WebGPUTexture texture("TestTexture");
    REQUIRE(texture.label() == "TestTexture");
    REQUIRE(texture.id() != 0);
    
    WebGPUBindGroup bindGroup("TestBindGroup");
    REQUIRE(bindGroup.label() == "TestBindGroup");
    REQUIRE(bindGroup.id() != 0);
    
    WebGPURenderPipeline pipeline("TestPipeline");
    REQUIRE(pipeline.label() == "TestPipeline");
    REQUIRE(pipeline.id() != 0);
    
    // Verify unique IDs
    REQUIRE(buffer.id() != texture.id());
    REQUIRE(texture.id() != bindGroup.id());
    REQUIRE(bindGroup.id() != pipeline.id());
  }
}