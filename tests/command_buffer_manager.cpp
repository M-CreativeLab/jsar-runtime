/**
 * Unit tests for TrCommandBufferManager
 *
 * Note: These tests focus on the API surface and thread safety.
 * Full integration tests requiring actual GPU device and encoding
 * implementation will be added once the WebGPU layer is complete.
 */

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <thread>
#include <vector>
#include "../src/renderer/command_buffer_manager.hpp"
#include "../src/common/command_buffers/base.hpp"
#include "../src/common/command_buffers/shared.hpp"

using namespace renderer;
using namespace commandbuffers;

// Simple mock command buffer for testing
class MockCommandBuffer : public TrCommandBufferBase
{
public:
  MockCommandBuffer(CommandBufferType type)
      : TrCommandBufferBase(type, sizeof(MockCommandBuffer))
  {
  }
};

/**
 * Note: Construction tests with actual GPUDeviceBase require a properly
 * initialized device with adapter, which is complex to mock.
 * These tests will be enabled once the GPU layer has factory methods.
 *
 * For now, we test the command classification logic which doesn't require
 * actual device initialization.
 */

TEST_CASE("TrCommandBufferManager null device validation", "[command_buffer_manager]")
{
  SECTION("Construction with null device throws")
  {
    REQUIRE_THROWS_AS(TrCommandBufferManager(nullptr), std::invalid_argument);
  }
}

/**
 * Tests for CommandTypes helper functions used by TrCommandBufferManager
 */
TEST_CASE("CommandTypes classification", "[command_buffer_manager][command_types]")
{
  SECTION("IsFramebufferDependentCommand identifies draw commands")
  {
    REQUIRE(CommandTypes::IsFramebufferDependentCommand(COMMAND_BUFFER_DRAW_ARRAYS_REQ));
    REQUIRE(CommandTypes::IsFramebufferDependentCommand(COMMAND_BUFFER_DRAW_ELEMENTS_REQ));
    REQUIRE(CommandTypes::IsFramebufferDependentCommand(COMMAND_BUFFER_CLEAR_REQ));
    REQUIRE(CommandTypes::IsFramebufferDependentCommand(COMMAND_BUFFER_SET_VIEWPORT_REQ));
  }

  SECTION("IsResourceCreatingCommand identifies resource commands")
  {
    REQUIRE(CommandTypes::IsResourceCreatingCommand(COMMAND_BUFFER_CREATE_BUFFER_REQ));
    REQUIRE(CommandTypes::IsResourceCreatingCommand(COMMAND_BUFFER_CREATE_TEXTURE_REQ));
    REQUIRE(CommandTypes::IsResourceCreatingCommand(COMMAND_BUFFER_CREATE_SHADER_REQ));
    REQUIRE(CommandTypes::IsResourceCreatingCommand(COMMAND_BUFFER_CREATE_PROGRAM_REQ));
  }

  SECTION("IsXRFrameControl identifies XR control commands")
  {
    REQUIRE(CommandTypes::IsXRFrameControl(COMMAND_BUFFER_XRFRAME_START_REQ));
    REQUIRE(CommandTypes::IsXRFrameControl(COMMAND_BUFFER_XRFRAME_FLUSH_REQ));
    REQUIRE(CommandTypes::IsXRFrameControl(COMMAND_BUFFER_XRFRAME_END_REQ));
  }

  SECTION("Non-classified commands return false")
  {
    REQUIRE_FALSE(CommandTypes::IsFramebufferDependentCommand(COMMAND_BUFFER_GET_ERROR_REQ));
    REQUIRE_FALSE(CommandTypes::IsResourceCreatingCommand(COMMAND_BUFFER_GET_ERROR_REQ));
    REQUIRE_FALSE(CommandTypes::IsXRFrameControl(COMMAND_BUFFER_GET_ERROR_REQ));
  }
}

TEST_CASE("EncodedPass move semantics", "[command_buffer_manager]")
{
  SECTION("Move constructor")
  {
    EncodedPass pass1;
    pass1.isFramebufferDependent = true;
    pass1.isResourceCreating = false;
    pass1.isXRPass = true;
    pass1.renderingInfo = xr::TrXRFrameRenderingInfo(1, 2, 0);

    EncodedPass pass2(std::move(pass1));
    REQUIRE(pass2.isFramebufferDependent == true);
    REQUIRE(pass2.isResourceCreating == false);
    REQUIRE(pass2.isXRPass == true);
    REQUIRE(pass2.renderingInfo.sessionId == 1);
  }

  SECTION("Move assignment")
  {
    EncodedPass pass1;
    pass1.isFramebufferDependent = true;

    EncodedPass pass2;
    pass2 = std::move(pass1);
    REQUIRE(pass2.isFramebufferDependent == true);
  }
}
