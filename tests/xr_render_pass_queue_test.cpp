#include <assert.h>
#include <stdio.h>
#include <iostream>
#include "xr/frame.hpp"
#include "xr/render_pass_queue_builder.hpp"
#include "common/command_buffers/shared.hpp"
#include "common/command_buffers/base.hpp"

using namespace std;
using namespace xr;
using namespace commandbuffers;

// Simple test command buffer request for testing
class TestCommandBufferRequest : public TrCommandBufferRequest
{
public:
  TestCommandBufferRequest() : TrCommandBufferRequest(COMMAND_BUFFER_UNKNOWN, sizeof(TestCommandBufferRequest))
  {
  }
  
  string toString(const char* prefix = "") const override
  {
    return "TestCommandBufferRequest";
  }
};

void testStereoRenderingFrameRenderPassEncoders()
{
  cout << "Testing StereoRenderingFrame render pass encoder functionality..." << endl;
  
  // Create a stereo rendering frame
  StereoRenderingFrame frame(true, 1);
  
  // Test initial state
  assert(frame.getRenderPassEncoders().empty());
  
  // Add a render pass encoder
  GPURenderPassEncoder encoder("TestEncoder");
  frame.addRenderPassEncoder(std::move(encoder));
  
  // Verify it was added
  assert(frame.getRenderPassEncoders().size() == 1);
  assert(frame.getRenderPassEncoders()[0].label == "TestEncoder");
  
  // Clear render pass encoders
  frame.clearRenderPassEncoders();
  assert(frame.getRenderPassEncoders().empty());
  
  cout << "✅ StereoRenderingFrame render pass encoder tests passed!" << endl;
}

void testXRRenderPassQueueBuilder()
{
  cout << "Testing XRRenderPassQueueBuilder functionality..." << endl;
  
  // Create a queue builder
  XRRenderPassQueueBuilder builder;
  
  // Test initial state
  assert(!builder.hasRequests());
  assert(builder.getRequestCount() == 0);
  
  // Add a command buffer request
  auto* request = new TestCommandBufferRequest();
  builder.addCommandBufferRequest(request);
  
  // Verify request was added
  assert(builder.hasRequests());
  assert(builder.getRequestCount() == 1);
  
  // Build render pass queue
  auto renderPassQueue = builder.buildRenderPassQueue();
  assert(renderPassQueue.size() == 1);
  
  // Clear the builder
  builder.clear();
  assert(!builder.hasRequests());
  assert(builder.getRequestCount() == 0);
  
  // Clean up the request since we created it
  delete request;
  
  cout << "✅ XRRenderPassQueueBuilder tests passed!" << endl;
}

void testIntegration()
{
  cout << "Testing integration between components..." << endl;
  
  // Create a stereo rendering frame
  StereoRenderingFrame frame(true, 2);
  
  // Create a queue builder
  XRRenderPassQueueBuilder builder;
  
  // Add multiple command buffer requests
  std::vector<TestCommandBufferRequest*> requests;
  for (int i = 0; i < 3; i++)
  {
    auto* request = new TestCommandBufferRequest();
    requests.push_back(request);
    builder.addCommandBufferRequest(request);
  }
  
  // Build render pass queue and add to frame
  auto renderPassQueue = builder.buildRenderPassQueue();
  for (auto& encoder : renderPassQueue)
  {
    frame.addRenderPassEncoder(std::move(encoder));
  }
  
  // Verify integration
  assert(frame.getRenderPassEncoders().size() == 3);
  
  // Clean up the requests since we created them
  for (auto* request : requests)
  {
    delete request;
  }
  
  cout << "✅ Integration tests passed!" << endl;
}

int main()
{
  cout << "Running XR Render Pass Queue tests..." << endl;
  
  try
  {
    testStereoRenderingFrameRenderPassEncoders();
    testXRRenderPassQueueBuilder();
    testIntegration();
    
    cout << "🎉 All tests passed successfully!" << endl;
    return 0;
  }
  catch (const exception& e)
  {
    cout << "❌ Test failed with exception: " << e.what() << endl;
    return 1;
  }
  catch (...)
  {
    cout << "❌ Test failed with unknown exception" << endl;
    return 1;
  }
}